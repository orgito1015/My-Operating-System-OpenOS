/*
 * OpenOS - Process Management Shell Commands (Phase 1)
 *
 *   ps        - list processes (pid, ppid, state, prio, cpu, name)
 *   kill      - terminate a process by pid
 *   spawn     - launch demo kernel threads (workers)
 *   forktest  - launch a ring 3 program that fork()s and wait()s
 *   user      - launch a ring 3 hello-world program
 *   counters  - launch N ring 3 counter programs concurrently
 *   psleep    - put the shell to sleep for N ms (scheduler demo)
 *   sched     - scheduler statistics
 */

#include "commands.h"
#include "shell.h"
#include "string.h"
#include "user_programs.h"
#include "../drivers/console.h"
#include "../process/process.h"
#include "../process/scheduler.h"

/* ------------------------------------------------------------------ */
/* Local formatting helpers                                             */
/* ------------------------------------------------------------------ */

static void write_dec(uint32_t v) {
    char buf[12];
    int pos = 0;
    do {
        buf[pos++] = (char)('0' + (v % 10));
        v /= 10;
    } while (v > 0);
    while (pos > 0) console_put_char(buf[--pos]);
}

static void write_dec_pad(uint32_t v, int width) {
    /* Compute digit count */
    uint32_t t = v;
    int digits = 0;
    do { digits++; t /= 10; } while (t > 0);
    for (int i = digits; i < width; i++) console_put_char(' ');
    write_dec(v);
}

static void write_str_pad(const char *s, int width) {
    int len = 0;
    console_write(s);
    while (s[len]) len++;
    for (int i = len; i < width; i++) console_put_char(' ');
}

static int parse_uint(const char *s, uint32_t *out) {
    if (!s || !*s) return -1;
    uint32_t v = 0;
    for (int i = 0; s[i]; i++) {
        if (s[i] < '0' || s[i] > '9') return -1;
        v = v * 10 + (uint32_t)(s[i] - '0');
    }
    *out = v;
    return 0;
}

/* ------------------------------------------------------------------ */
/* Demo kernel-thread workload for `spawn`                              */
/* ------------------------------------------------------------------ */

static void worker_thread(void *arg) {
    uint32_t rounds = (uint32_t)arg;
    if (rounds == 0 || rounds > 10) rounds = 3;

    for (uint32_t i = 1; i <= rounds; i++) {
        console_write("[worker pid ");
        write_dec(process_getpid());
        console_write("] round ");
        write_dec(i);
        console_write("/");
        write_dec(rounds);
        console_write("\n");
        process_sleep(150);
    }
    console_write("[worker pid ");
    write_dec(process_getpid());
    console_write("] finished\n");
    /* Returning triggers process_thread_exit() -> process_exit(0). */
}

/* ------------------------------------------------------------------ */
/* Commands                                                             */
/* ------------------------------------------------------------------ */

void cmd_ps(int argc, char **argv) {
    (void)argc; (void)argv;

    console_write("\n  PID  PPID  STATE     PRI  CPU(ticks)  MODE   NAME\n");
    console_write("  ---  ----  --------  ---  ----------  -----  ----------------\n");

    for (int i = 0; i < PROCESS_MAX; i++) {
        process_t *p = process_table_entry(i);
        if (!p || p->state == PROCESS_STATE_UNUSED) continue;

        console_write("  ");
        write_dec_pad(p->pid, 3);
        console_write("  ");
        write_dec_pad(p->ppid, 4);
        console_write("  ");
        write_str_pad(process_state_name(p->state), 8);
        console_write("  ");
        write_dec_pad(p->priority, 3);
        console_write("  ");
        write_dec_pad((uint32_t)p->cpu_ticks, 10);
        console_write("  ");
        write_str_pad(p->is_user ? "user" : "kern", 5);
        console_write("  ");
        console_write(p->name);
        console_write("\n");
    }

    console_write("\n  ");
    write_dec((uint32_t)process_count());
    console_write(" processes\n\n");
}

void cmd_kill(int argc, char **argv) {
    if (argc < 2) {
        console_write("Usage: kill <pid>\n");
        return;
    }
    uint32_t pid;
    if (parse_uint(argv[1], &pid) != 0) {
        console_write("kill: invalid pid\n");
        return;
    }
    if (pid == process_getpid()) {
        console_write("kill: refusing to kill the shell\n");
        return;
    }
    if (process_kill(pid) == 0) {
        console_write("Process ");
        write_dec(pid);
        console_write(" terminated\n");
    } else {
        console_write("kill: no such process\n");
    }
}

void cmd_spawn(int argc, char **argv) {
    uint32_t count = 2;
    if (argc >= 2 && parse_uint(argv[1], &count) != 0) {
        console_write("Usage: spawn [count]\n");
        return;
    }
    if (count == 0 || count > 8) count = 2;

    for (uint32_t i = 0; i < count; i++) {
        uint8_t prio = (uint8_t)(PRIORITY_NORMAL + (i % 2)); /* mix N/L */
        process_t *p = process_create("worker", worker_thread,
                                      (void *)3u, prio);
        if (p) {
            console_write("Spawned worker pid ");
            write_dec(p->pid);
            console_write(" (priority ");
            write_dec(prio);
            console_write(")\n");
        } else {
            console_write("spawn: process_create failed\n");
            return;
        }
    }
    console_write("Run 'ps' to watch them; they print concurrently.\n");
}

void cmd_forktest(int argc, char **argv) {
    (void)argc; (void)argv;
    process_t *p = process_create_user("forktest", uprog_forktest,
                                       PRIORITY_NORMAL);
    if (!p) {
        console_write("forktest: failed to create user process\n");
        return;
    }
    console_write("Launched ring 3 fork test as pid ");
    write_dec(p->pid);
    console_write("\n");
}

void cmd_user(int argc, char **argv) {
    (void)argc; (void)argv;
    process_t *p = process_create_user("uhello", uprog_hello,
                                       PRIORITY_NORMAL);
    if (!p) {
        console_write("user: failed to create user process\n");
        return;
    }
    console_write("Launched ring 3 program as pid ");
    write_dec(p->pid);
    console_write("\n");
}

void cmd_counters(int argc, char **argv) {
    uint32_t count = 2;
    if (argc >= 2 && parse_uint(argv[1], &count) != 0) {
        console_write("Usage: counters [count]\n");
        return;
    }
    if (count == 0 || count > 6) count = 2;

    for (uint32_t i = 0; i < count; i++) {
        process_t *p = process_create_user("ucounter", uprog_counter,
                                           PRIORITY_NORMAL);
        if (p) {
            console_write("Launched ring 3 counter pid ");
            write_dec(p->pid);
            console_write("\n");
        }
    }
}

void cmd_psleep(int argc, char **argv) {
    uint32_t ms = 1000;
    if (argc >= 2 && parse_uint(argv[1], &ms) != 0) {
        console_write("Usage: psleep [ms]\n");
        return;
    }
    if (ms > 10000) ms = 10000;

    console_write("Shell sleeping ");
    write_dec(ms);
    console_write(" ms (other processes keep running)...\n");
    process_sleep(ms);
    console_write("Shell awake.\n");
}

void cmd_sched(int argc, char **argv) {
    (void)argc; (void)argv;

    sched_stats_t st;
    scheduler_get_stats(&st);

    console_write("\nScheduler: preemptive multilevel round-robin\n");
    console_write("  Quantum:           ");
    write_dec(SCHED_QUANTUM_TICKS * 10);
    console_write(" ms (");
    write_dec(SCHED_QUANTUM_TICKS);
    console_write(" ticks @ 100 Hz)\n");
    console_write("  Aging threshold:   ");
    write_dec(AGING_THRESHOLD);
    console_write(" ticks\n");
    console_write("  Context switches:  ");
    write_dec((uint32_t)st.context_switches);
    console_write("\n");
    console_write("  Timer ticks:       ");
    write_dec((uint32_t)st.ticks);
    console_write("\n");
    console_write("  Ready queues:      high=");
    write_dec(st.ready_count[PRIORITY_HIGH]);
    console_write(" normal=");
    write_dec(st.ready_count[PRIORITY_NORMAL]);
    console_write(" low=");
    write_dec(st.ready_count[PRIORITY_LOW]);
    console_write("\n\n");
}
