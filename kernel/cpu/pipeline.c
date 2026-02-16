/*
 * OpenOS - 5-Stage Pipelined CPU Simulator
 * 
 * Implementation of a classic RISC 5-stage pipeline.
 * 
 * EDUCATIONAL NOTE: This is a structural pipeline simulation focused on
 * demonstrating pipeline mechanics, hazard detection, and performance
 * characteristics (CPI, stalls). It does not implement full ALU operations
 * or data computation. The purpose is to show how instructions flow through
 * pipeline stages and how hazards affect performance.
 */

#include "pipeline.h"
#include <stddef.h>

/* Simple memset implementation for freestanding environment */
static void *simple_memset(void *s, int c, size_t n) {
    unsigned char *p = (unsigned char *)s;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return s;
}

/* Initialize pipelined CPU */
void pipeline_init(PipelineCPU *cpu) {
    simple_memset(cpu, 0, sizeof(PipelineCPU));
    cpu->running = true;
    
    /* Mark all stages as empty initially */
    for (int i = 0; i < STAGE_COUNT; i++) {
        cpu->stages[i].instr.valid = false;
        cpu->stages[i].stalled = false;
    }
}

/* Reset CPU state */
void pipeline_reset(PipelineCPU *cpu) {
    pipeline_init(cpu);
}

/* Fetch instruction from memory */
static Instruction fetch_instruction(uint32_t *memory, uint32_t mem_size, uint32_t pc) {
    Instruction instr;
    simple_memset(&instr, 0, sizeof(Instruction));
    
    if (pc / 4 >= mem_size) {
        instr.valid = false;
        return instr;
    }
    
    uint32_t raw = memory[pc / 4];
    instr.opcode = raw & 0x7F;
    instr.rd = (raw >> 7) & 0x1F;
    instr.rs1 = (raw >> 15) & 0x1F;
    instr.rs2 = (raw >> 20) & 0x1F;
    instr.immediate = raw >> 20;
    instr.pc = pc;
    instr.valid = true;
    
    return instr;
}

/* Hazard detection: simplified for educational purposes */
bool pipeline_detect_hazard(const PipelineCPU *cpu) {
    /* For a simple educational pipeline, we'll use minimal hazard detection */
    /* Check for RAW (Read-After-Write) data hazard only with adjacent instructions */
    if (cpu->stages[STAGE_ID].instr.valid && cpu->stages[STAGE_EX].instr.valid) {
        Instruction id_instr = cpu->stages[STAGE_ID].instr;
        Instruction ex_instr = cpu->stages[STAGE_EX].instr;
        
        /* Only stall if there's a true dependency and destination is used */
        if (ex_instr.rd != 0 && ex_instr.opcode != 0x23 &&  /* Not a store */
            (id_instr.rs1 == ex_instr.rd || id_instr.rs2 == ex_instr.rd)) {
            return true;
        }
    }
    
    return false;
}

/* Execute one clock cycle */
void pipeline_cycle(PipelineCPU *cpu, uint32_t *memory, uint32_t mem_size) {
    if (!cpu->running) return;
    
    bool stall = pipeline_detect_hazard(cpu);
    
    /* Create temporary storage for next cycle state */
    PipelineStage next_stages[STAGE_COUNT];
    for (int i = 0; i < STAGE_COUNT; i++) {
        next_stages[i].instr.valid = false;  /* Clear by default */
    }
    
    /* WB stage: Write back to register file */
    if (cpu->stages[STAGE_WB].instr.valid) {
        Instruction instr = cpu->stages[STAGE_WB].instr;
        if (instr.rd != 0 && instr.opcode != 0x23) {  /* Not a store */
            /* Note: This is a simplified educational simulation.
             * In a real pipeline, the result would be computed in EX stage
             * and carried through MEM to WB. Here we use immediate as a
             * placeholder to demonstrate the pipeline flow without full
             * ALU implementation. */
            cpu->registers[instr.rd] = instr.immediate;
        }
        cpu->instruction_count++;
        /* WB completes, stage becomes empty */
    }
    
    /* MEM stage: Memory access, always move to WB */
    if (cpu->stages[STAGE_MEM].instr.valid) {
        next_stages[STAGE_WB].instr = cpu->stages[STAGE_MEM].instr;
    }
    
    /* EX stage: Execute operation, move to MEM if not stalling */
    if (cpu->stages[STAGE_EX].instr.valid) {
        next_stages[STAGE_MEM].instr = cpu->stages[STAGE_EX].instr;
    }
    
    /* ID stage: Decode instruction, move to EX if not stalling */
    if (cpu->stages[STAGE_ID].instr.valid) {
        if (!stall) {
            next_stages[STAGE_EX].instr = cpu->stages[STAGE_ID].instr;
        } else {
            /* Stall: keep instruction in ID */
            next_stages[STAGE_ID].instr = cpu->stages[STAGE_ID].instr;
            cpu->stall_count++;
        }
    }
    
    /* IF stage: Fetch instruction, move to ID if not stalling */
    if (!stall && cpu->pc / 4 < mem_size) {
        Instruction instr = fetch_instruction(memory, mem_size, cpu->pc);
        if (instr.valid) {
            next_stages[STAGE_ID].instr = instr;
            cpu->pc += 4;  /* Move to next instruction */
        }
    } else if (cpu->pc / 4 >= mem_size) {
        /* No more instructions to fetch, check if pipeline is empty */
        if (!cpu->stages[STAGE_ID].instr.valid && 
            !cpu->stages[STAGE_EX].instr.valid &&
            !cpu->stages[STAGE_MEM].instr.valid &&
            !cpu->stages[STAGE_WB].instr.valid) {
            cpu->running = false;  /* Pipeline is empty */
        }
    }
    
    /* Update stages for next cycle */
    for (int i = 0; i < STAGE_COUNT; i++) {
        cpu->stages[i] = next_stages[i];
    }
    
    cpu->cycle_count++;
}

/* Execute N instructions */
void pipeline_execute(PipelineCPU *cpu, uint32_t *memory, uint32_t mem_size, uint32_t num_instructions) {
    uint64_t target_instructions = cpu->instruction_count + num_instructions;
    
    while (cpu->running && cpu->instruction_count < target_instructions) {
        pipeline_cycle(cpu, memory, mem_size);
        
        /* Safety check: prevent infinite loop */
        if (cpu->cycle_count > target_instructions * 10) {
            break;
        }
    }
}

/* Get CPI (Cycles Per Instruction) */
double pipeline_get_cpi(const PipelineCPU *cpu) {
    if (cpu->instruction_count == 0) return 0.0;
    return (double)cpu->cycle_count / (double)cpu->instruction_count;
}

/* Get total cycles */
uint64_t pipeline_get_cycles(const PipelineCPU *cpu) {
    return cpu->cycle_count;
}

/* Get instructions executed */
uint64_t pipeline_get_instructions(const PipelineCPU *cpu) {
    return cpu->instruction_count;
}

/* Get stalls */
uint64_t pipeline_get_stalls(const PipelineCPU *cpu) {
    return cpu->stall_count;
}
