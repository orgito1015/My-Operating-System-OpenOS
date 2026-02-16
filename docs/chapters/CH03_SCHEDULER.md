# Chapter 03: Process Scheduler

## Phase Alignment
Phase 1 - Process Management (Planned)

## Objectives
- Design and implement process scheduling algorithm
- Implement context switching mechanism
- Create process state management
- Implement time-slice based preemption
- Establish process priority system

## Concepts Studied

### Scheduling Algorithms
- Round-robin scheduling
- Priority-based scheduling
- Multilevel queue scheduling
- Completely Fair Scheduler (CFS)
- Real-time scheduling

### Process States
- Running: currently executing
- Ready: waiting for CPU
- Blocked: waiting for I/O or event
- Zombie: terminated but not reaped
- State transitions and lifecycle

### Context Switching
- Register saving and restoration
- Stack pointer management
- Page directory switching
- TLB flushing considerations
- Performance optimization

### Time Management
- Timer interrupt driven scheduling
- Time quantum (time slice)
- Scheduler tick frequency
- CPU time accounting
- Sleep and wakeup mechanisms

### Preemption
- Voluntary vs involuntary preemption
- Preemption points
- Critical sections and atomicity
- Interrupt handling during preemption

## Implementation Status

### Completed
- None (Phase 1 not yet started)

### In Progress
- None

### Pending
- Process control block (PCB) structure
- Process queue management
- Round-robin scheduler implementation
- Context switch routine (assembly)
- Scheduler entry point
- Process creation and termination
- Time slice management
- Idle process implementation

## Open Questions

### Architectural
- Which scheduling algorithm to implement first?
- Should we support priority levels from the start?
- How to handle real-time processes?

### Implementation
- What is the optimal time slice duration?
- How to minimize context switch overhead?
- Should we implement scheduler classes?

### Design Decisions
- Fixed priority vs dynamic priority?
- Single queue vs per-CPU queues?
- How to prevent priority inversion?
- Should we implement CPU affinity?

### Performance
- What is acceptable context switch latency?
- How to measure scheduler fairness?
- Trade-offs between throughput and latency?

## Next Actions

### Immediate
- Define process control block structure
- Design process queue data structure
- Plan context switch register layout

### Short Term
- Implement basic round-robin scheduler
- Create context switch assembly routine
- Add scheduler statistics tracking

### Long Term
- Implement priority-based scheduling
- Add support for CPU affinity
- Design real-time scheduling extensions
- Implement load balancing for SMP

## Research Topics

### Scheduler Design
- Linux CFS (Completely Fair Scheduler)
- Windows thread scheduler
- FreeBSD ULE scheduler
- Real-time operating systems (RTOS)

### Context Switch Optimization
- Lazy FPU context switching
- TLB shootdown minimization
- Cache-aware scheduling
- ASID (Address Space Identifier) usage

### Advanced Features
- Gang scheduling
- Proportional share scheduling
- Energy-aware scheduling
- NUMA-aware scheduling

## Current Challenges

### Design Phase
- Choosing appropriate scheduling algorithm
- Balancing simplicity with functionality
- Ensuring fair CPU allocation

### Implementation Complexity
- Context switch requires careful register management
- Race conditions in scheduler code
- Interrupt handling during scheduling

### Performance Concerns
- Context switch overhead
- Cache pollution effects
- Lock contention in scheduler

## References
- "Operating System Concepts" by Silberschatz et al.: Chapter 5
- Linux kernel scheduler documentation
- "Understanding the Linux Kernel" by Bovet & Cesati: Chapter 7
- "Inside the Linux Scheduler" by Mosberger & Eranian
- xv6 scheduler implementation
- Minix 3 scheduler design

## Notes
The scheduler is the heart of process management. Design decisions made here affect system responsiveness, throughput, and fairness. Start with a simple round-robin implementation, then iterate based on observed behavior.

Context switching is one of the most performance-critical operations in an OS. Minimize overhead by careful register management and avoiding unnecessary TLB flushes.

Consider implementing a simple idle process that runs when no other process is ready. This simplifies scheduler logic by ensuring there is always something to run.
