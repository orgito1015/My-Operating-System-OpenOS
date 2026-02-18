/*
 * OpenOS - SMP Implementation
 */

#include "smp.h"
#include "console.h"
#include "string.h"

/* Global SMP information */
static smp_info_t smp_system;
static int smp_initialized = 0;

/* CPUID detection helper */
static inline int cpuid_available(void) {
    uint32_t eax, edx;
    __asm__ volatile (
        "pushfl\n"
        "pop %%eax\n"
        "mov %%eax, %%edx\n"
        "xor $0x200000, %%eax\n"
        "push %%eax\n"
        "popfl\n"
        "pushfl\n"
        "pop %%eax\n"
        : "=a" (eax), "=d" (edx)
    );
    return (eax != edx);
}

/* Get CPUID information */
static inline void cpuid(uint32_t code, uint32_t* a, uint32_t* d) {
    __asm__ volatile ("cpuid" : "=a"(*a), "=d"(*d) : "a"(code) : "ebx", "ecx");
}

/* Detect number of CPUs */
static uint32_t detect_cpu_count(void) {
    if (!cpuid_available()) {
        return 1; /* CPUID not available, assume single CPU */
    }
    
    uint32_t eax, edx;
    cpuid(1, &eax, &edx);
    
    /* Check for multi-threading support */
    if (edx & (1 << 28)) {
        /* HTT bit is set, check logical processor count
         * Note: This returns the maximum number of addressable IDs for logical
         * processors in this physical package. This may include hyper-threading
         * logical processors, not just physical cores. For a more accurate
         * physical core count, CPUID leaf 0x0B or 0x1F would be needed.
         */
        uint32_t logical_count = (edx >> 16) & 0xFF;
        if (logical_count > 1) {
            return logical_count;
        }
    }
    
    return 1; /* Single CPU */
}

/* Initialize SMP subsystem */
void smp_init(void) {
    if (smp_initialized) return;
    
    console_write("SMP: Detecting CPUs...\n");
    
    /* Initialize SMP structure */
    smp_system.cpu_count = detect_cpu_count();
    smp_system.bsp_id = 0; /* BSP is always CPU 0 */
    
    /* Initialize CPU info structures */
    for (uint32_t i = 0; i < MAX_CPUS; i++) {
        smp_system.cpus[i].cpu_id = i;
        smp_system.cpus[i].state = CPU_STATE_OFFLINE;
        smp_system.cpus[i].apic_id = i;
        smp_system.cpus[i].flags = 0;
        smp_system.cpus[i].tsc_freq = 0;
    }
    
    /* Mark BSP as online */
    smp_system.cpus[0].state = CPU_STATE_ONLINE;
    
    /* Print detection results */
    console_write("SMP: Detected ");
    char buf[16];
    itoa(smp_system.cpu_count, buf, 10);
    console_write(buf);
    console_write(" CPU(s)\n");
    
    smp_initialized = 1;
}

/* Get number of CPUs */
uint32_t smp_get_cpu_count(void) {
    return smp_system.cpu_count;
}

/* Get current CPU ID (simplified - always return 0 for now) */
uint32_t smp_get_current_cpu(void) {
    return 0; /* TODO: Read from APIC */
}

/* Get CPU info */
cpu_info_t* smp_get_cpu_info(uint32_t cpu_id) {
    if (cpu_id >= MAX_CPUS) return NULL;
    return &smp_system.cpus[cpu_id];
}

/* Boot Application Processor */
int smp_boot_ap(uint32_t cpu_id) {
    if (cpu_id >= smp_system.cpu_count || cpu_id == 0) {
        return -1; /* Invalid CPU ID or trying to boot BSP */
    }
    
    /* TODO: Implement AP boot sequence with APIC */
    smp_system.cpus[cpu_id].state = CPU_STATE_ONLINE;
    
    console_write("SMP: Booted CPU ");
    char buf[16];
    itoa(cpu_id, buf, 10);
    console_write(buf);
    console_write("\n");
    
    return 0;
}

/* Halt a CPU */
void smp_halt_cpu(uint32_t cpu_id) {
    if (cpu_id >= MAX_CPUS) return;
    
    smp_system.cpus[cpu_id].state = CPU_STATE_HALTED;
    
    if (cpu_id == smp_get_current_cpu()) {
        /* Halt this CPU */
        __asm__ volatile ("cli; hlt");
    }
}
