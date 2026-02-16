# Chapter 04: Process Model

## Phase Alignment
Phase 1 - Process Management (Planned)

## Objectives
- Design process abstraction and lifecycle
- Implement process creation and termination
- Implement fork() system call
- Establish parent-child relationships
- Create process table management

## Concepts Studied

### Process Abstraction
- Process vs thread distinction
- Process address space layout
- Process control block (PCB) structure
- Process ID (PID) allocation
- Process hierarchy

### Process Creation
- fork() semantics and implementation
- Copy-on-write (COW) optimization
- exec() family of system calls
- Process initialization sequence
- Address space duplication

### Process Termination
- exit() system call
- Process cleanup and resource release
- Zombie processes and reaping
- Orphan process handling
- Wait and waitpid semantics

### Address Space Management
- User vs kernel space separation
- Memory layout: text, data, heap, stack
- Shared memory regions
- Memory protection and isolation
- Address space switching

### Inter-Process Relationships
- Parent-child relationships
- Process groups and sessions
- Process tree structure
- Orphan and zombie handling
- Init process role

## Implementation Status

### Completed
- None (Phase 1 not yet started)

### In Progress
- None

### Pending
- Process control block (PCB) design
- Process table implementation
- fork() system call
- Process creation routine
- Address space cloning
- Process termination handling
- PID allocation mechanism
- Wait/waitpid implementation
- Zombie process reaping
- Init process setup

## Open Questions

### Architectural
- Should we implement threads from the start?
- How to handle process limits?
- Static vs dynamic PID allocation?

### fork() Implementation
- Full copy vs copy-on-write?
- How to handle fork failure?
- Should we limit fork depth?

### Address Space
- What is the user/kernel split ratio?
- How to handle stack growth?
- Should we implement automatic stack expansion?

### Process Lifecycle
- Who reaps zombie processes if parent dies?
- How to handle init process termination?
- Should we implement process groups initially?

## Next Actions

### Immediate
- Define PCB structure with all necessary fields
- Design process table data structure
- Plan memory layout for user processes

### Short Term
- Implement basic process creation
- Add fork() system call
- Create process termination handling
- Implement wait() functionality

### Long Term
- Add exec() system call family
- Implement copy-on-write for fork
- Add process signals
- Create process debugging support

## Research Topics

### Process Creation
- Linux fork/clone implementation
- Windows CreateProcess approach
- Plan 9 rfork system call
- Spawn vs fork+exec

### Copy-on-Write
- Page fault based COW
- Reference counting for pages
- COW performance characteristics
- Optimizing fork() overhead

### Process Management
- Process descriptor design patterns
- Efficient PID allocation schemes
- Process table lookup optimization
- Resource tracking strategies

## Current Challenges

### Design Phase
- Balancing PCB size with information needs
- Efficient process lookup mechanisms
- Resource limit enforcement

### Implementation Complexity
- fork() requires sophisticated memory management
- Race conditions in process creation
- Handling fork failures cleanly

### Memory Management
- Address space cloning is complex
- Copy-on-write adds significant complexity
- Stack and heap management for multiple processes

## Implementation Details

### Process Control Block (PCB)
Proposed structure:
- Process ID (PID)
- Parent process ID (PPID)
- Process state (running, ready, blocked, zombie)
- CPU registers (for context switching)
- Page directory pointer
- Memory regions (text, data, heap, stack)
- Open file descriptors
- Signal handlers
- Process priority
- CPU time consumed
- Working directory

### Process Table
- Array or hash table of PCBs
- Fast PID to PCB lookup
- Efficient iteration for scheduling
- Lock-free or fine-grained locking

### Memory Layout (Typical)
```
0xFFFFFFFF ┌─────────────────┐
           │ Kernel Space    │
0xC0000000 ├─────────────────┤
           │ Stack (grows ↓) │
           │                 │
           │ Unused          │
           │                 │
           │ Heap (grows ↑)  │
           ├─────────────────┤
           │ BSS             │
           ├─────────────────┤
           │ Data            │
           ├─────────────────┤
           │ Text (code)     │
0x08048000 └─────────────────┘
```

## References
- "Operating System Concepts" by Silberschatz et al.: Chapters 3-4
- Linux kernel process management
- "Understanding the Linux Kernel" by Bovet & Cesati: Chapter 3
- "The Design and Implementation of the 4.4BSD OS" by McKusick et al.
- xv6 process implementation
- Minix 3 process management

## Notes
The process is the fundamental abstraction in Unix-like systems. Get this right, and everything else follows more naturally. Start simple with single-threaded processes, add complexity incrementally.

fork() is conceptually simple but implementation is complex. Consider starting with a full-copy implementation, then optimize with COW once the basic mechanism works.

Pay special attention to the init process (PID 1), as it has special responsibilities for reaping orphaned processes.
