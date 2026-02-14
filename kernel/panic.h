/*
 * OpenOS - Kernel Panic Handler
 * 
 * Handles unrecoverable kernel errors with detailed diagnostic information.
 */

#ifndef OPENOS_KERNEL_PANIC_H
#define OPENOS_KERNEL_PANIC_H

/* Kernel panic - halts the system with an error message */
void kernel_panic(const char* message);

/* Kernel panic with additional context information */
void kernel_panic_ext(const char* message, const char* file, int line);

/* Convenience macro for panic with file and line info */
#define PANIC(msg) kernel_panic_ext(msg, __FILE__, __LINE__)

#endif /* OPENOS_KERNEL_PANIC_H */
