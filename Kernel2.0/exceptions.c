/*
 * OpenOS - Exception Handler Implementation
 * Handles CPU exceptions with detailed error reporting
 */

#include "exceptions.h"
#include "idt.h"
#include <stddef.h>

/* Forward declarations */
void terminal_write(const char* s);
void terminal_put_char(char c);

/* Exception names for error reporting */
static const char* exception_messages[] = {
    "Divide by Zero",
    "Debug",
    "Non-Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 FPU Error",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved"
};

/* Helper function to print a hexadecimal number */
static void print_hex(uint32_t value) {
    const char hex_digits[] = "0123456789ABCDEF";
    char buffer[11];  /* "0x" + 8 hex digits + null terminator */
    buffer[0] = '0';
    buffer[1] = 'x';
    
    for (int i = 9; i >= 2; i--) {
        buffer[i] = hex_digits[value & 0xF];
        value >>= 4;
    }
    buffer[10] = '\0';
    
    terminal_write(buffer);
}

/* Helper function to print a decimal number */
static void print_dec(uint32_t value) {
    if (value == 0) {
        terminal_put_char('0');
        return;
    }
    
    char buffer[11];  /* Max 10 digits + null terminator */
    int i = 10;
    buffer[i] = '\0';
    
    while (value > 0 && i > 0) {
        i--;
        buffer[i] = '0' + (value % 10);
        value /= 10;
    }
    
    terminal_write(&buffer[i]);
}

/*
 * Main exception handler called from assembly stubs
 */
void exception_handler(struct exception_registers *regs) {
    /* Print exception header */
    terminal_write("\n");
    terminal_write("======================================\n");
    terminal_write("    KERNEL PANIC - EXCEPTION!\n");
    terminal_write("======================================\n");
    terminal_write("\n");
    
    /* Print exception type */
    terminal_write("Exception: ");
    if (regs->int_no < 32) {
        terminal_write(exception_messages[regs->int_no]);
    } else {
        terminal_write("Unknown");
    }
    terminal_write(" (");
    print_dec(regs->int_no);
    terminal_write(")\n");
    
    /* Print error code if present */
    terminal_write("Error Code: ");
    print_hex(regs->err_code);
    terminal_write("\n\n");
    
    /* Special handling for page faults */
    if (regs->int_no == EXCEPTION_PAGE_FAULT) {
        uint32_t faulting_address;
        __asm__ __volatile__("mov %%cr2, %0" : "=r"(faulting_address));
        
        terminal_write("Page Fault Details:\n");
        terminal_write("  Faulting Address: ");
        print_hex(faulting_address);
        terminal_write("\n");
        
        terminal_write("  Cause: ");
        if (!(regs->err_code & 0x1)) terminal_write("Page not present ");
        if (regs->err_code & 0x2) terminal_write("Write access ");
        else terminal_write("Read access ");
        if (regs->err_code & 0x4) terminal_write("(User mode)");
        else terminal_write("(Kernel mode)");
        terminal_write("\n\n");
    }
    
    /* Print register dump */
    terminal_write("Register Dump:\n");
    terminal_write("  EAX="); print_hex(regs->eax);
    terminal_write("  EBX="); print_hex(regs->ebx);
    terminal_write("  ECX="); print_hex(regs->ecx);
    terminal_write("  EDX="); print_hex(regs->edx);
    terminal_write("\n");
    
    terminal_write("  ESI="); print_hex(regs->esi);
    terminal_write("  EDI="); print_hex(regs->edi);
    terminal_write("  EBP="); print_hex(regs->ebp);
    terminal_write("  ESP="); print_hex(regs->esp);
    terminal_write("\n");
    
    terminal_write("  EIP="); print_hex(regs->eip);
    terminal_write("  CS="); print_hex(regs->cs);
    terminal_write("  DS="); print_hex(regs->ds);
    terminal_write("  EFLAGS="); print_hex(regs->eflags);
    terminal_write("\n\n");
    
    /* Print stack information */
    terminal_write("Stack Segment: ");
    print_hex(regs->ss);
    terminal_write("\n");
    terminal_write("User ESP: ");
    print_hex(regs->useresp);
    terminal_write("\n\n");
    
    terminal_write("======================================\n");
    terminal_write("System Halted - Cannot Continue\n");
    terminal_write("======================================\n");
    
    /* Halt the system */
    __asm__ __volatile__("cli; hlt");
    
    /* Infinite loop in case CPU wakes up */
    while(1) {
        __asm__ __volatile__("hlt");
    }
}

/*
 * Initialize exception handlers by installing them in the IDT
 */
void exceptions_init(void) {
    /* Install all 32 exception handlers in the IDT */
    /* Use kernel code segment (0x08) and interrupt gate flags (0x8E) */
    idt_set_gate(0, (uint32_t)exception_0, 0x08, 0x8E);
    idt_set_gate(1, (uint32_t)exception_1, 0x08, 0x8E);
    idt_set_gate(2, (uint32_t)exception_2, 0x08, 0x8E);
    idt_set_gate(3, (uint32_t)exception_3, 0x08, 0x8E);
    idt_set_gate(4, (uint32_t)exception_4, 0x08, 0x8E);
    idt_set_gate(5, (uint32_t)exception_5, 0x08, 0x8E);
    idt_set_gate(6, (uint32_t)exception_6, 0x08, 0x8E);
    idt_set_gate(7, (uint32_t)exception_7, 0x08, 0x8E);
    idt_set_gate(8, (uint32_t)exception_8, 0x08, 0x8E);
    idt_set_gate(9, (uint32_t)exception_9, 0x08, 0x8E);
    idt_set_gate(10, (uint32_t)exception_10, 0x08, 0x8E);
    idt_set_gate(11, (uint32_t)exception_11, 0x08, 0x8E);
    idt_set_gate(12, (uint32_t)exception_12, 0x08, 0x8E);
    idt_set_gate(13, (uint32_t)exception_13, 0x08, 0x8E);
    idt_set_gate(14, (uint32_t)exception_14, 0x08, 0x8E);
    idt_set_gate(15, (uint32_t)exception_15, 0x08, 0x8E);
    idt_set_gate(16, (uint32_t)exception_16, 0x08, 0x8E);
    idt_set_gate(17, (uint32_t)exception_17, 0x08, 0x8E);
    idt_set_gate(18, (uint32_t)exception_18, 0x08, 0x8E);
    idt_set_gate(19, (uint32_t)exception_19, 0x08, 0x8E);
    idt_set_gate(20, (uint32_t)exception_20, 0x08, 0x8E);
    idt_set_gate(21, (uint32_t)exception_21, 0x08, 0x8E);
    idt_set_gate(22, (uint32_t)exception_22, 0x08, 0x8E);
    idt_set_gate(23, (uint32_t)exception_23, 0x08, 0x8E);
    idt_set_gate(24, (uint32_t)exception_24, 0x08, 0x8E);
    idt_set_gate(25, (uint32_t)exception_25, 0x08, 0x8E);
    idt_set_gate(26, (uint32_t)exception_26, 0x08, 0x8E);
    idt_set_gate(27, (uint32_t)exception_27, 0x08, 0x8E);
    idt_set_gate(28, (uint32_t)exception_28, 0x08, 0x8E);
    idt_set_gate(29, (uint32_t)exception_29, 0x08, 0x8E);
    idt_set_gate(30, (uint32_t)exception_30, 0x08, 0x8E);
    idt_set_gate(31, (uint32_t)exception_31, 0x08, 0x8E);
}
