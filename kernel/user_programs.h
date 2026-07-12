/*
 * OpenOS - Userland Demo Programs (Phase 1)
 *
 * Small ring 3 programs compiled into the kernel image and launched
 * via process_create_user(). They interact with the kernel exclusively
 * through int 0x80 syscalls (see include/usyscall.h).
 */

#ifndef OPENOS_KERNEL_USER_PROGRAMS_H
#define OPENOS_KERNEL_USER_PROGRAMS_H

/* Prints greetings via sys_write, sleeps, and exits. */
void uprog_hello(void);

/* fork() demo: parent forks a child, both print, parent wait()s. */
void uprog_forktest(void);

/* Counter that yields between iterations (scheduler demo). */
void uprog_counter(void);

#endif /* OPENOS_KERNEL_USER_PROGRAMS_H */
