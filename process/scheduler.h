/*
 * OpenOS - Scheduler (Phase 1)
 *
 * Preemptive multilevel round-robin scheduler:
 *   - Three priority levels (HIGH / NORMAL / LOW), each a FIFO ready
 *     queue. The scheduler always runs the head of the highest
 *     non-empty queue; processes at the same level share the CPU
 *     round-robin via a fixed time quantum.
 *   - Aging: a READY process that waits longer than AGING_THRESHOLD
 *     ticks is temporarily boosted one level so low-priority work
 *     cannot starve. The boost is undone when it runs.
 *   - Timed sleep: SLEEPING processes are woken by the timer tick.
 *
 * Preemption is driven by the PIT: timer_handler() calls
 * scheduler_tick() on every tick (after sending EOI); when the running
 * process's quantum expires, schedule() switches inside the interrupt
 * context. The preempted process's IRET frame stays on its own kernel
 * stack and unwinds when it is next resumed.
 */

#ifndef OPENOS_PROCESS_SCHEDULER_H
#define OPENOS_PROCESS_SCHEDULER_H

#include "process.h"

/* Time quantum per process, in timer ticks (100 Hz -> 10 ms/tick) */
#define SCHED_QUANTUM_TICKS  5

/* READY ticks after which a process is boosted one priority level */
#define AGING_THRESHOLD      200

/* Start preemptive scheduling. Call after process_init() and after at
 * least the idle process (PID 0) exists. */
void scheduler_start(void);

/* True once scheduler_start() has run. */
int scheduler_active(void);

/* Called from the timer interrupt on every tick (interrupts off). */
void scheduler_tick(void);

/* Add a process to the ready queue for its priority. */
void scheduler_enqueue(process_t *p);

/* Remove a process from whatever ready queue holds it (no-op if absent). */
void scheduler_dequeue(process_t *p);

/* Pick the next process and context switch to it.
 * Must be called with interrupts disabled. */
void schedule(void);

/* Voluntarily give up the CPU (safe to call with interrupts enabled). */
void scheduler_yield(void);

/* Block the current process (state must be set by caller) and switch. */
void scheduler_block_current(void);

/* Make a blocked/sleeping process runnable again. */
void scheduler_unblock(process_t *p);

/* Statistics for `sched` shell command. */
typedef struct {
    uint64_t context_switches;
    uint64_t ticks;
    uint32_t ready_count[PRIORITY_LEVELS];
} sched_stats_t;

void scheduler_get_stats(sched_stats_t *out);

#endif /* OPENOS_PROCESS_SCHEDULER_H */
