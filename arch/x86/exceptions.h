/*
 * OpenOS - Exception Handlers
 * Handles CPU exceptions (faults, traps, aborts)
 */

#ifndef OPENOS_ARCH_X86_EXCEPTIONS_H
#define OPENOS_ARCH_X86_EXCEPTIONS_H

#include <stdint.h>

/* CPU Exception Numbers */
#define EXCEPTION_DIVIDE_ERROR           0
#define EXCEPTION_DEBUG                  1
#define EXCEPTION_NMI                    2
#define EXCEPTION_BREAKPOINT             3
#define EXCEPTION_OVERFLOW               4
#define EXCEPTION_BOUND_RANGE            5
#define EXCEPTION_INVALID_OPCODE         6
#define EXCEPTION_DEVICE_NOT_AVAILABLE   7
#define EXCEPTION_DOUBLE_FAULT           8
#define EXCEPTION_COPROCESSOR_SEGMENT    9
#define EXCEPTION_INVALID_TSS            10
#define EXCEPTION_SEGMENT_NOT_PRESENT    11
#define EXCEPTION_STACK_SEGMENT_FAULT    12
#define EXCEPTION_GENERAL_PROTECTION     13
#define EXCEPTION_PAGE_FAULT             14
#define EXCEPTION_RESERVED_15            15
#define EXCEPTION_X87_FPU_ERROR          16
#define EXCEPTION_ALIGNMENT_CHECK        17
#define EXCEPTION_MACHINE_CHECK          18
#define EXCEPTION_SIMD_FP_EXCEPTION      19
#define EXCEPTION_VIRTUALIZATION         20
#define EXCEPTION_CONTROL_PROTECTION     21
/* 22-27 Reserved */
#define EXCEPTION_HYPERVISOR_INJECTION   28
#define EXCEPTION_VMM_COMMUNICATION      29
#define EXCEPTION_SECURITY_EXCEPTION     30
/* 31 Reserved */

/* Interrupt frame pushed by CPU on exception */
struct interrupt_frame {
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
} __attribute__((packed));

/* Registers saved by exception handler */
struct exception_registers {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} __attribute__((packed));

/* Exception handler function type */
typedef void (*exception_handler_t)(struct exception_registers *regs);

/* Initialize exception handlers */
void exceptions_init(void);

/* Assembly exception stubs (defined in exceptions.S) */
void exception_0(void);
void exception_1(void);
void exception_2(void);
void exception_3(void);
void exception_4(void);
void exception_5(void);
void exception_6(void);
void exception_7(void);
void exception_8(void);
void exception_9(void);
void exception_10(void);
void exception_11(void);
void exception_12(void);
void exception_13(void);
void exception_14(void);
void exception_15(void);
void exception_16(void);
void exception_17(void);
void exception_18(void);
void exception_19(void);
void exception_20(void);
void exception_21(void);
void exception_22(void);
void exception_23(void);
void exception_24(void);
void exception_25(void);
void exception_26(void);
void exception_27(void);
void exception_28(void);
void exception_29(void);
void exception_30(void);
void exception_31(void);

#endif /* OPENOS_ARCH_X86_EXCEPTIONS_H */
