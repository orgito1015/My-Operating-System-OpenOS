/*
 * OpenOS - Process Management (Phase 1)
 *
 * Process control blocks, kernel threads, user-mode processes,
 * fork/exit/wait/kill/sleep, and process table management.
 *
 * Every process owns a kernel stack. Kernel threads execute entirely on
 * it; user processes additionally own a user stack and enter ring 3 via
 * enter_user_mode(). Context state between switches is a small
 * callee-saved frame on the kernel stack (see arch/x86/context.S).
 */

#ifndef OPENOS_PROCESS_PROCESS_H
#define OPENOS_PROCESS_PROCESS_H

#include <stdint.h>
#include <stddef.h>

/* Limits */
#define PROCESS_MAX          64
#define PROCESS_NAME_LEN     32
#define PROCESS_KSTACK_SIZE  16384   /* 16 KiB kernel stack   */
#define PROCESS_USTACK_SIZE  16384   /* 16 KiB user stack     */

/* Priorities (lower number = higher priority) */
#define PRIORITY_HIGH    0
#define PRIORITY_NORMAL  1
#define PRIORITY_LOW     2
#define PRIORITY_LEVELS  3

/* Process states */
typedef enum {
    PROCESS_STATE_UNUSED = 0,   /* Slot free                            */
    PROCESS_STATE_READY,        /* Runnable, waiting in a ready queue   */
    PROCESS_STATE_RUNNING,      /* Currently executing                  */
    PROCESS_STATE_BLOCKED,      /* Waiting on an event (e.g. wait())    */
    PROCESS_STATE_SLEEPING,     /* Timed sleep until sleep_until ticks  */
    PROCESS_STATE_ZOMBIE,       /* Exited; awaiting reaping             */
    PROCESS_STATE_TERMINATED = PROCESS_STATE_ZOMBIE  /* Legacy alias    */
} process_state_t;

/* Process entry point type (kernel threads) */
typedef void (*process_entry_t)(void *arg);

/* Process control block */
typedef struct process {
    uint32_t         pid;
    uint32_t         ppid;
    char             name[PROCESS_NAME_LEN];
    process_state_t  state;

    /* Scheduling */
    uint8_t          priority;       /* 0 (high) .. 2 (low)             */
    uint8_t          base_priority;  /* Priority before aging boosts    */
    uint32_t         quantum_left;   /* Ticks left in current quantum   */
    uint32_t         wait_ticks;     /* Ticks spent waiting (aging)     */
    uint64_t         cpu_ticks;      /* Total ticks of CPU time         */
    struct process  *next;           /* Ready-queue link                */

    /* Kernel stack + saved context */
    uint8_t         *kstack;         /* Base of kernel stack            */
    uint32_t         kstack_top;     /* Top (initial ESP0)              */
    uint32_t         esp;            /* Saved kernel ESP when not running */

    /* User mode */
    int              is_user;
    uint8_t         *ustack;         /* Base of user stack (0 if kernel) */
    uint32_t         ustack_top;
    uint32_t         user_entry;     /* Ring 3 entry point               */

    /* Sleep / exit bookkeeping */
    uint64_t         sleep_until;    /* Wake tick when SLEEPING          */
    int              exit_code;
    int              parent_waiting; /* Parent blocked in process_wait() */
} process_t;

/* ---- Lifecycle ---------------------------------------------------- */

/* Initialize process management; adopts the boot context as PID 0 (idle). */
void process_init(void);

/* Create a kernel thread. Returns PCB or NULL. */
process_t *process_create(const char *name, process_entry_t entry,
                          void *arg, uint8_t priority);

/* Create a ring 3 process running `entry` on its own user stack. */
process_t *process_create_user(const char *name, void (*entry)(void),
                               uint8_t priority);

/* Terminate the calling process. Never returns. */
void process_exit(int code) __attribute__((noreturn));

/* Terminate an arbitrary process by pid. Returns 0 on success. */
int process_kill(uint32_t pid);

/* Block until a child exits; reaps it. Returns child pid or -1.
 * If status != NULL, receives the child's exit code. */
int process_wait(int *status);

/* Sleep the calling process for at least `ms` milliseconds. */
void process_sleep(uint32_t ms);

/* ---- Queries ------------------------------------------------------ */

process_t *process_current(void);
uint32_t   process_getpid(void);
uint32_t   process_getppid(void);
process_t *process_by_pid(uint32_t pid);
process_t *process_table_entry(int index);      /* index 0..PROCESS_MAX-1 */
const char *process_state_name(process_state_t s);
int         process_count(void);

/* Internal: mark table entry free and release its stacks (reaper). */
void process_release(process_t *p);

#endif /* OPENOS_PROCESS_PROCESS_H */
