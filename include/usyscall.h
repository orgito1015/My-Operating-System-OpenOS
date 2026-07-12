/*
 * OpenOS - Userland System Call Wrappers
 *
 * Inline int 0x80 wrappers for code that runs in ring 3. These are the
 * ONLY way user-mode code may enter the kernel; calling kernel
 * functions directly from ring 3 works only because Phase 1 has no
 * memory protection yet, and is forbidden by convention.
 */

#ifndef OPENOS_INCLUDE_USYSCALL_H
#define OPENOS_INCLUDE_USYSCALL_H

#include <stdint.h>
#include "../kernel/syscall.h"

static inline int _syscall0(int num) {
    int ret;
    __asm__ __volatile__("int $0x80"
                         : "=a"(ret)
                         : "a"(num)
                         : "memory");
    return ret;
}

static inline int _syscall1(int num, uint32_t a1) {
    int ret;
    __asm__ __volatile__("int $0x80"
                         : "=a"(ret)
                         : "a"(num), "b"(a1)
                         : "memory");
    return ret;
}

static inline void u_exit(int code) {
    _syscall1(SYS_EXIT, (uint32_t)code);
    for (;;) { }   /* unreachable */
}

static inline int  u_write(const char *s)  { return _syscall1(SYS_WRITE, (uint32_t)s); }
static inline int  u_getpid(void)          { return _syscall0(SYS_GETPID); }
static inline int  u_getppid(void)         { return _syscall0(SYS_GETPPID); }
static inline int  u_fork(void)            { return _syscall0(SYS_FORK); }
static inline int  u_yield(void)           { return _syscall0(SYS_YIELD); }
static inline int  u_sleep(uint32_t ms)    { return _syscall1(SYS_SLEEP, ms); }
static inline int  u_wait(int *status)     { return _syscall1(SYS_WAIT, (uint32_t)status); }

#endif /* OPENOS_INCLUDE_USYSCALL_H */
