/*
 * OpenOS - Userland Demo Programs (Phase 1)
 *
 * These functions run at CPL 3. They must not call kernel functions
 * directly; everything goes through int 0x80. Small helpers (number
 * formatting) are implemented locally so no kernel code is touched.
 */

#include "user_programs.h"
#include "../include/usyscall.h"

/* Format an int into a caller-provided buffer (ring 3 helper). */
static void u_itoa(int value, char *buf) {
    char tmp[12];
    int pos = 0, neg = 0;
    unsigned int v;

    if (value < 0) {
        neg = 1;
        v = (unsigned int)(-value);
    } else {
        v = (unsigned int)value;
    }
    do {
        tmp[pos++] = (char)('0' + (v % 10));
        v /= 10;
    } while (v > 0);
    int i = 0;
    if (neg) buf[i++] = '-';
    while (pos > 0) buf[i++] = tmp[--pos];
    buf[i] = '\0';
}

/* ------------------------------------------------------------------ */

void uprog_hello(void) {
    char pid_buf[12];
    u_itoa(u_getpid(), pid_buf);

    u_write("[user] hello from ring 3! pid=");
    u_write(pid_buf);
    u_write("\n[user] sleeping 300 ms via sys_sleep...\n");
    u_sleep(300);
    u_write("[user] awake again, exiting via sys_exit\n");
    u_exit(0);
}

/* ------------------------------------------------------------------ */

void uprog_forktest(void) {
    char buf[12];

    u_write("[fork] parent pid=");
    u_itoa(u_getpid(), buf);
    u_write(buf);
    u_write(" calling fork()...\n");

    int pid = u_fork();

    if (pid == 0) {
        /* Child */
        u_write("[fork]   child: fork() returned 0, my pid=");
        u_itoa(u_getpid(), buf);
        u_write(buf);
        u_write(", ppid=");
        u_itoa(u_getppid(), buf);
        u_write(buf);
        u_write("\n[fork]   child: doing some work...\n");
        u_sleep(200);
        u_write("[fork]   child: exiting with code 42\n");
        u_exit(42);
    } else if (pid > 0) {
        /* Parent */
        u_write("[fork] parent: fork() returned child pid=");
        u_itoa(pid, buf);
        u_write(buf);
        u_write("\n[fork] parent: waiting for child...\n");

        int status = 0;
        int reaped = u_wait(&status);

        u_write("[fork] parent: wait() reaped pid=");
        u_itoa(reaped, buf);
        u_write(buf);
        u_write(" exit code=");
        u_itoa(status, buf);
        u_write(buf);
        u_write("\n");
        u_exit(0);
    } else {
        u_write("[fork] fork() failed\n");
        u_exit(1);
    }
}

/* ------------------------------------------------------------------ */

void uprog_counter(void) {
    char buf[12];
    char pid_buf[12];
    u_itoa(u_getpid(), pid_buf);

    for (int i = 1; i <= 5; i++) {
        u_write("[counter pid ");
        u_write(pid_buf);
        u_write("] tick ");
        u_itoa(i, buf);
        u_write(buf);
        u_write("\n");
        u_sleep(100);
    }
    u_write("[counter pid ");
    u_write(pid_buf);
    u_write("] done\n");
    u_exit(0);
}
