# OpenOS Documentation

This directory contains the complete documentation for the OpenOS project, organized into logical sections for different purposes and audiences.

## Documentation Structure

### /architecture
Core architectural documentation describing the system design, module organization, and technical implementation details.

**Contents:**
- `ARCHITECTURE.md` - Modular monolithic architecture overview
- `system_overview.md` - System architecture and boot process details

**Purpose:** Reference documentation for understanding the kernel's internal structure and design principles.

**Audience:** Developers working on kernel internals, contributors, and those studying the codebase.

### /boot
Documentation related to the boot process, bootloaders, and multiboot compliance.

**Contents:**
- `MULTIBOOT_FIX.md` - Technical deep-dive into GRUB multiboot header fix
- `MULTIBOOT_FIX_SUMMARY.md` - Summary of multiboot fixes
- `MULTIBOOT_BEFORE_AFTER.md` - Comparison of multiboot implementations

**Purpose:** Detailed documentation of boot process implementation and troubleshooting.

**Audience:** Those debugging boot issues or implementing bootloader support.

### /roadmap
Strategic planning documents, evolution strategies, and phase completion records.

**Contents:**
- `roadmap.md` - Future development plans and feature roadmap
- `OS_EVOLUTION_STRATEGY.md` - 36-week development roadmap
- `UPGRADE_PHASE0.md` - Complete Phase 0 implementation guide

**Purpose:** Track project direction, milestones, and long-term vision.

**Audience:** Project maintainers, contributors planning features, stakeholders.

### /refactoring
Documentation of major refactoring efforts, code reorganizations, and migration guides.

**Contents:**
- `REFACTORING.md` - Refactoring guide and migration information
- `KERNEL_REFACTORING_SUMMARY.md` - Summary of kernel refactoring efforts

**Purpose:** Historical record of structural changes and rationale behind them.

**Audience:** Contributors understanding codebase evolution, migration guides for developers.

### /virtualization
Guides for running OpenOS in virtualized environments.

**Contents:**
- `VIRTUALBOX_QUICKSTART.md` - Quick start guide for VirtualBox
- `VIRTUALBOX_TROUBLESHOOTING.md` - VirtualBox troubleshooting guide

**Purpose:** Practical guides for setting up and debugging virtualized environments.

**Audience:** Users and developers testing OpenOS in VMs.

### /chapters
Structured learning materials organized by topic, tracking knowledge progression through OS concepts.

**Contents:**
- `CH00_FOUNDATIONS.md` - Boot process and kernel fundamentals
- `CH01_MEMORY.md` - Memory management (PMM, VMM, heap)
- `CH02_INTERRUPTS.md` - Interrupts and exception handling
- `CH03_SCHEDULER.md` - Process scheduling (planned)
- `CH04_PROCESS_MODEL.md` - Process management (planned)
- `CH05_SECURITY.md` - Security and protection (future)
- `CH06_RUST.md` - Rust integration research (future)

**Purpose:** Educational documentation tracking concept learning and implementation progress.

**Audience:** Learners studying OS development, contributors onboarding to the project.

### /learning
Learning control files and knowledge progression tracking.

**Contents:**
- `LEARNING_INDEX.md` - Current phase, focus areas, completed chapters, and learning resources

**Purpose:** Central hub for tracking learning progress and identifying knowledge gaps.

**Audience:** Self-learners, students, and educators using OpenOS for teaching.

### /journal
Development journal documenting day-to-day progress, learnings, and implementation notes.

**Contents:**
- `DEVELOPMENT_LOG.md` - Chronological log of development activities

**Purpose:** Historical record of development process, decisions, and lessons learned.

**Audience:** Project maintainers, future developers understanding development history.

### /research
Research notes, experimental designs, and investigation documentation.

**Contents:**
- (Future) Research papers, experiment results, performance studies

**Purpose:** Document research activities, prototypes, and experimental features.

**Audience:** Researchers, advanced contributors exploring new approaches.

## Documentation Types Explained

### Architecture vs Chapters
- **Architecture**: Describes *what* the system is and *how* it works (implementation-focused)
- **Chapters**: Describes *what* was learned and *why* decisions were made (learning-focused)

Architecture documentation is stable and changes only when implementation changes. Chapter documentation evolves with understanding and may include speculation, open questions, and research topics.

### Roadmap vs Journal
- **Roadmap**: Forward-looking plans, milestones, and strategic direction
- **Journal**: Backward-looking record of what actually happened, day-by-day

Roadmap is aspirational and subject to change. Journal is factual and immutable.

### Refactoring vs Architecture
- **Refactoring**: Documents *changes* to the architecture and migration paths
- **Architecture**: Documents the *current* architecture as it exists now

Refactoring docs are historical and explain transitions. Architecture docs describe present state.

## Documentation Standards

### Writing Style
- **Professional tone**: Clear, precise, technical language
- **No emojis**: Keep documentation professional and timeless
- **Active voice**: Prefer "The kernel initializes memory" over "Memory is initialized"
- **Present tense**: Document current state in present tense
- **Imperative for instructions**: "Run make" not "You should run make"

### Markdown Formatting
- Use ATX-style headers (`#` not underlines)
- Code blocks must specify language: ```c not ```
- Use tables for structured data
- Use lists for sequential or unordered items
- Use horizontal rules (`---`) to separate major sections

### Code Examples
```c
// Good: Include context and comments
void example_function(void) {
    // Clear explanation of what this does
    some_operation();
}
```

### Diagrams
Use ASCII art for simple diagrams:
```
Kernel Space  ┌─────────────┐
              │   Kernel    │
              ├─────────────┤
User Space    │   Process   │
              └─────────────┘
```

For complex diagrams, consider external tools and link images.

## Naming Conventions

### File Names
- Use UPPERCASE for major documents: `ARCHITECTURE.md`, `REFACTORING.md`
- Use snake_case for descriptive documents: `system_overview.md`
- Use UPPERCASE for document abbreviations: `README.md`, `TODO.md`
- Chapters use format: `CH00_TOPIC.md`

### Section Headers
- Capitalize major words in headers: "Memory Management" not "memory management"
- Use consistent hierarchy: # for page title, ## for major sections, ### for subsections

### Code References
- Use backticks for code elements: `kmalloc()`, `struct pcb`
- Use full paths when referencing files: `arch/x86/idt.c`
- Link to specific lines when helpful: `kernel/kernel.c:42`

## How to Add New Documentation

### New Feature Documentation
1. Create entry in `/journal/DEVELOPMENT_LOG.md` during development
2. Update relevant file in `/architecture` when feature is complete
3. Add to appropriate chapter in `/chapters` if it represents new learning
4. Update `/learning/LEARNING_INDEX.md` if completing a chapter milestone

### New Chapter
1. Create `CHxx_TOPIC.md` in `/chapters` following existing template
2. Include: Phase Alignment, Objectives, Concepts Studied, Implementation Status, Open Questions, Next Actions
3. Add entry to `/learning/LEARNING_INDEX.md`

### New Research Topic
1. Create document in `/research` with descriptive name
2. Include: motivation, approach, findings, conclusions, future work
3. Reference from relevant chapter or learning index

### Refactoring Documentation
1. Document the change in `/refactoring` with before/after descriptions
2. Update affected architecture documents
3. Add migration guide if needed
4. Log in journal with reasoning

## Documentation Review Process

### Before Committing
- Spell check all documents
- Verify all links work
- Ensure code examples compile (if applicable)
- Check markdown renders correctly
- Validate formatting consistency

### Periodic Review
- Quarterly: Review and update learning index
- After each phase: Update architecture docs
- Monthly: Review journal for patterns and lessons
- Annually: Archive old documentation if needed

## Documentation Metrics

Current documentation status:
- Total markdown files: 25+
- Documentation lines: 30,000+
- Chapters completed: 3/7
- Architecture docs: Complete for Phase 0
- Code-to-docs ratio: ~15:1 (healthy)

## Contributing to Documentation

See `CONTRIBUTING.md` in the repository root for general contribution guidelines.

### Documentation-Specific Guidelines
- Document as you code, not after
- Prefer small, focused documents over monoliths
- Link related documents together
- Update existing docs when making changes
- Add examples and diagrams where helpful
- Explain *why*, not just *what*

### Quality Standards
- No broken links
- No spelling errors
- Consistent formatting
- Clear and concise writing
- Accurate technical information
- Up-to-date with codebase

## Getting Help

### Finding Information
1. Check `/learning/LEARNING_INDEX.md` for topic overview
2. Read relevant chapter for conceptual understanding
3. Consult architecture docs for implementation details
4. Review journal for historical context
5. Check roadmap for future plans

### Documentation Issues
- Found a broken link? Please fix it or report it
- Documentation unclear? Open an issue with suggestions
- Missing documentation? Please add it or request it
- Outdated information? Update it or flag it

## External References

### Essential Reading
- Intel Software Developer Manual (Vol 1-3)
- OSDev Wiki: https://wiki.osdev.org
- Linux Kernel Documentation
- xv6 Source Code and Commentary

### Recommended Books
- "Operating System Concepts" by Silberschatz et al.
- "Understanding the Linux Kernel" by Bovet & Cesati
- "The Design and Implementation of the 4.4BSD Operating System"
- "Modern Operating Systems" by Tanenbaum

### Online Resources
- MIT 6.828: Operating System Engineering
- Writing an OS in Rust (blog series)
- OSDev Forums and Community

---

## Quick Links

### Start Here
- [Learning Index](learning/LEARNING_INDEX.md) - Current status and learning path
- [Architecture Overview](architecture/ARCHITECTURE.md) - System design
- [Development Journal](journal/DEVELOPMENT_LOG.md) - Recent progress

### For Developers
- [Refactoring Guide](refactoring/REFACTORING.md) - Code migration
- [Phase 0 Completion](roadmap/UPGRADE_PHASE0.md) - Foundation details

### For Users
- [VirtualBox Quickstart](virtualization/VIRTUALBOX_QUICKSTART.md) - Getting started
- [Roadmap](roadmap/roadmap.md) - Future features

---

*Documentation structure established: [Date]*
*Last reviewed: [Date]*
*Next review: After Phase 1 completion*
