/*
 * OpenOS - Scheduler Implementation (Phase 1)
 *
 * Multilevel round-robin with aging. See scheduler.h for the policy
 * description. All queue manipulation happens with interrupts disabled;
 * schedule() itself must only ever be entered with interrupts off
 * (either from the timer interrupt, or after an explicit cli).
 */

#include "scheduler.h"
#include "process.h"
#include "../arch/x86/gdt.h"
#include "../drivers/timer.h"
#include "../drivers/console.h"

/* From arch/x86/context.S */
extern void context_switch(uint32_t *old_esp, uint32_t new_esp);

/* From process.c */
extern process_t *current_process;

/* One FIFO ready queue per priority level */
static process_t *queue_head[PRIORITY_LEVELS];
static process_t *queue_tail[PRIORITY_LEVELS];

static int      started = 0;
static uint64_t context_switches = 0;

/* ------------------------------------------------------------------ */
/* Ready queues                                                         */
/* ------------------------------------------------------------------ */

void scheduler_enqueue(process_t *p) {
    if (!p || p->pid == 0) return;          /* idle never queues */

    p->next       = 0;
    p->wait_ticks = 0;

    uint8_t q = p->priority;
    if (q >= PRIORITY_LEVELS) q = PRIORITY_LEVELS - 1;

    if (queue_tail[q]) {
        queue_tail[q]->next = p;
        queue_tail[q] = p;
    } else {
        queue_head[q] = queue_tail[q] = p;
    }
}

void scheduler_dequeue(process_t *p) {
    if (!p) return;
    for (int q = 0; q < PRIORITY_LEVELS; q++) {
        process_t *prev = 0;
        for (process_t *it = queue_head[q]; it; prev = it, it = it->next) {
            if (it != p) continue;
            if (prev) prev->next = it->next;
            else      queue_head[q] = it->next;
            if (queue_tail[q] == it) queue_tail[q] = prev;
            it->next = 0;
            return;
        }
    }
}

/* Pop the head of the highest-priority non-empty queue. */
static process_t *pick_next(void) {
    for (int q = 0; q < PRIORITY_LEVELS; q++) {
        process_t *p = queue_head[q];
        if (p) {
            queue_head[q] = p->next;
            if (!queue_head[q]) queue_tail[q] = 0;
            p->next = 0;
            return p;
        }
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* Housekeeping run inside schedule()                                   */
/* ------------------------------------------------------------------ */

/*
 * Free the stacks of zombies nobody is waiting for. A zombie whose
 * parent is still alive is kept (its exit code may be collected via
 * process_wait()); once the parent is gone it is released. The process
 * that just called process_exit() is skipped while it is still
 * `current` — its kernel stack is in use until we switch away.
 */
static void reap_orphans(void) {
    for (int i = 0; i < PROCESS_MAX; i++) {
        process_t *p = process_table_entry(i);
        if (!p || p->state != PROCESS_STATE_ZOMBIE) continue;
        if (p == current_process) continue;

        process_t *parent = process_by_pid(p->ppid);
        if (!parent || parent->state == PROCESS_STATE_UNUSED ||
            parent->state == PROCESS_STATE_ZOMBIE ||
            parent->pid == 0 /* children of idle/kernel auto-reap */) {
            process_release(p);
        }
    }
}

/* Boost long-waiting READY processes one level (anti-starvation). */
static void apply_aging(void) {
    for (int q = 1; q < PRIORITY_LEVELS; q++) {
        process_t *it = queue_head[q];
        while (it) {
            process_t *next = it->next;
            it->wait_ticks++;
            if (it->wait_ticks > AGING_THRESHOLD) {
                scheduler_dequeue(it);
                it->priority = (uint8_t)(q - 1);
                scheduler_enqueue(it);   /* resets wait_ticks */
            }
            it = next;
        }
    }
}

/* Wake sleepers whose deadline has passed. */
static void wake_sleepers(uint64_t now) {
    for (int i = 0; i < PROCESS_MAX; i++) {
        process_t *p = process_table_entry(i);
        if (!p || p->state != PROCESS_STATE_SLEEPING) continue;
        if (now >= p->sleep_until) {
            p->state = PROCESS_STATE_READY;
            scheduler_enqueue(p);
        }
    }
}

/* ------------------------------------------------------------------ */
/* Core                                                                 */
/* ------------------------------------------------------------------ */

void scheduler_start(void) {
    started = 1;
    console_write("Scheduler: preemptive round-robin active (quantum 50 ms)\n");
}

int scheduler_active(void) {
    return started;
}

void schedule(void) {
    if (!started) return;

    reap_orphans();

    process_t *prev = current_process;
    process_t *next = pick_next();

    if (!next) {
        /*
         * Nothing else is runnable. If the current process can keep
         * running, let it; otherwise fall back to the idle process.
         */
        if (prev->state == PROCESS_STATE_RUNNING) {
            prev->quantum_left = SCHED_QUANTUM_TICKS;
            return;
        }
        next = process_by_pid(0);
        if (!next || next == prev) {
            /* Idle blocked?! Should not happen; just keep running. */
            return;
        }
    }

    /* Requeue the outgoing process if it is still runnable. */
    if (prev->state == PROCESS_STATE_RUNNING) {
        prev->state = PROCESS_STATE_READY;
        /* Aging boost (if any) is spent: restore the base priority. */
        prev->priority = prev->base_priority;
        scheduler_enqueue(prev);
    }

    next->state        = PROCESS_STATE_RUNNING;
    next->priority     = next->base_priority;
    next->quantum_left = SCHED_QUANTUM_TICKS;

    current_process = next;
    context_switches++;

    /*
     * Point the TSS at the new process's kernel stack so the next
     * ring 3 -> ring 0 transition (interrupt or int 0x80) lands on it.
     */
    tss_set_kernel_stack(next->kstack_top ? next->kstack_top : 0);

    if (prev != next) {
        context_switch(&prev->esp, next->esp);
        /* Execution resumes here when `prev` is scheduled again. */
    }
}

void scheduler_tick(void) {
    if (!started) return;

    uint64_t now = timer_get_ticks();

    current_process->cpu_ticks++;

    wake_sleepers(now);
    apply_aging();

    /* Preempt when the quantum expires or a higher-priority process
     * became runnable. */
    int higher_ready = 0;
    for (int q = 0; q < current_process->priority; q++) {
        if (queue_head[q]) { higher_ready = 1; break; }
    }
    /* The idle process is preempted the moment anything is runnable. */
    if (current_process->pid == 0) {
        for (int q = 0; q < PRIORITY_LEVELS; q++) {
            if (queue_head[q]) { higher_ready = 1; break; }
        }
    }

    if (current_process->quantum_left > 0) {
        current_process->quantum_left--;
    }

    if (higher_ready || current_process->quantum_left == 0) {
        schedule();
    }
}

void scheduler_yield(void) {
    __asm__ __volatile__("cli");
    if (started) {
        current_process->quantum_left = 0;
        schedule();
    }
    __asm__ __volatile__("sti");
}

void scheduler_block_current(void) {
    /* Caller set current->state to BLOCKED/SLEEPING, interrupts off. */
    schedule();
}

void scheduler_unblock(process_t *p) {
    if (!p) return;
    if (p->state == PROCESS_STATE_BLOCKED ||
        p->state == PROCESS_STATE_SLEEPING) {
        p->state = PROCESS_STATE_READY;
        scheduler_enqueue(p);
    }
}

void scheduler_get_stats(sched_stats_t *out) {
    if (!out) return;
    out->context_switches = context_switches;
    out->ticks = timer_get_ticks();
    for (int q = 0; q < PRIORITY_LEVELS; q++) {
        uint32_t n = 0;
        for (process_t *it = queue_head[q]; it; it = it->next) n++;
        out->ready_count[q] = n;
    }
}
