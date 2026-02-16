# OpenOS CPU Architecture Simulator

This directory contains the CPU simulation components for OpenOS, providing educational implementations of different CPU architectures for performance comparison.

## Components

### 1. Pipelined CPU (`pipeline.c/h`)

A 5-stage RISC pipeline implementation:

- **IF** (Instruction Fetch): Fetches instructions from memory
- **ID** (Instruction Decode): Decodes instruction fields
- **EX** (Execute): Performs ALU operations
- **MEM** (Memory Access): Reads/writes memory
- **WB** (Write Back): Updates register file

**Features:**
- Basic hazard detection (RAW data hazards)
- Pipeline stall mechanism
- Performance counters (cycles, instructions, stalls)
- CPI calculation

**Performance:**
- Typical CPI: ~1.25 (with hazards)
- Benefits from instruction-level parallelism
- Lower throughput due to stalls

### 2. Single-Cycle CPU (`single_cycle.c/h`)

Reference implementation executing one instruction per cycle:

**Features:**
- No pipeline stages
- No hazards
- Simple, sequential execution
- CPI = 1.0 (by definition)

**Performance:**
- Guaranteed CPI: 1.0
- Higher MIPS for ideal conditions
- No stalls or complexity

### 3. Performance Counters (`performance.c/h`)

Unified performance tracking:

**Metrics:**
- Cycles Per Instruction (CPI)
- Instructions Per Cycle (IPC)
- MIPS (Million Instructions Per Second)
- Cache hit/miss rates
- Pipeline stalls

**Usage:**
```c
PerformanceCounters perf;
performance_init(&perf);
performance_update_cycles(&perf, cycles);
performance_update_instructions(&perf, instructions);
double cpi = performance_get_cpi(&perf);
double mips = performance_get_mips(&perf, clock_freq_mhz);
```

## Memory Subsystem

### 4. Direct-Mapped Cache (`memory/cache.c/h`)

**Configuration:**
- 256 cache lines
- 32 bytes per block
- 32-bit addressing

**Address Breakdown:**
- Offset: 5 bits (bytes within block)
- Index: 8 bits (cache line)
- Tag: 19 bits (block identification)

**Features:**
- Hit/miss tracking
- Write-through simulation
- Cache statistics

### 5. Memory Bus (`memory/bus.c/h`)

**Configuration:**
- 64-bit data width (8 bytes)
- 800 MHz frequency
- 30 ns memory access latency

**Features:**
- Bus arbitration simulation
- Throughput calculation
- Bandwidth utilization metrics

## Benchmark Program

Location: `benchmarks/pipeline_vs_single.c`

**Executes:**
- 20,000 instructions on each CPU
- Cache performance test (10,000 accesses)
- Memory bus performance test (1,000 transactions)

**Run:**
```bash
make benchmark
```

**Sample Output:**
```
OpenOS CPU Architecture Simulator
==================================

=== Pipelined CPU Benchmark ===
Instructions executed: 8192
Total cycles: 10245
Pipeline stalls: 2048
CPI: 1.251
MIPS: 799.61
Execution time: 0.000184 seconds

=== Single-Cycle CPU Benchmark ===
Instructions executed: 8192
Total cycles: 8192
CPI: 1.000
MIPS: 1000.00
Execution time: 0.000042 seconds

=== Cache Performance Benchmark ===
Cache accesses: 10000
Cache hits: 3316
Cache misses: 6684
Hit rate: 33.16%
Miss rate: 66.84%

=== Memory Bus Performance ===
Bus frequency: 800 MHz
Bus width: 8 bytes
Memory latency: 24 cycles (30.0 ns)
Read transactions: 500
Write transactions: 500
Total bytes: 8000
Throughput: 6103.52 MB/s
```

## Building

### Kernel with CPU Components
```bash
make all
```

The CPU simulation modules are compiled as part of the kernel but use a freestanding environment (no standard library).

### Benchmark (Hosted Application)
```bash
make benchmark
```

The benchmark is a hosted application that uses the standard library for I/O.

## Design Principles

1. **Modularity**: Each CPU model is independent
2. **Extensibility**: Easy to add new pipeline stages or CPU models
3. **Educational**: Clear comments explaining each stage
4. **Realistic**: Models real CPU behaviors (hazards, stalls, cache misses)
5. **Freestanding**: Kernel components work without standard library

## Future Extensions

- [ ] Data forwarding for hazard mitigation
- [ ] Branch prediction
- [ ] Out-of-order execution
- [ ] Multi-core simulation
- [ ] Set-associative cache
- [ ] Cache coherence protocols
- [ ] Superscalar pipeline (10+ stages)

## Performance Analysis

The pipelined CPU shows ~20% performance degradation compared to the single-cycle due to:
- Pipeline stalls from data hazards
- Structural hazards
- Control hazards (not yet implemented)

With forwarding and better hazard handling, the pipeline can achieve CPI < 1.1, making it more efficient than single-cycle for longer workloads.

## References

- Computer Architecture: A Quantitative Approach (Hennessy & Patterson)
- RISC-V Instruction Set Manual
- Computer Organization and Design (Patterson & Hennessy)
