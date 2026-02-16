# Contributing to OpenOS

Thank you for your interest in contributing to OpenOS! This is an educational operating system project, and we welcome contributions from developers at all skill levels.

## How to Contribute

### Reporting Issues
- Check if the issue already exists before creating a new one
- Provide detailed information about the bug or feature request
- Include steps to reproduce bugs
- Mention your development environment (OS, compiler version, etc.)

### Code Contributions

1. **Fork the Repository**
   - Fork the OpenOS repository to your GitHub account
   - Clone your fork locally

2. **Create a Branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```

3. **Make Your Changes**
   - Follow the coding standards outlined below
   - Keep changes focused and minimal
   - Test your changes thoroughly

4. **Commit Your Changes**
   - Write clear, descriptive commit messages
   - Reference issue numbers when applicable
   ```bash
   git commit -m "Add keyboard buffer overflow protection (#123)"
   ```

5. **Push and Create Pull Request**
   ```bash
   git push origin feature/your-feature-name
   ```
   - Create a pull request from your fork to the main repository
   - Provide a clear description of what your changes do

## Coding Standards

### C Code
- Use C99 standard (`-std=gnu99`)
- Use explicit integer types (`uint8_t`, `uint16_t`, `uint32_t`) instead of `int` for low-level code
- Use `size_t` for array indices and sizes
- Add comments for complex logic
- Keep functions small and focused
- Validate input parameters where appropriate

### Assembly Code
- Use clear, descriptive labels
- Add comments explaining each section
- Follow Intel syntax conventions

### Style Guidelines
- Indentation: 4 spaces (no tabs)
- Opening braces on same line for functions
- Clear variable names (no single-letter names except loop counters)
- Constants in UPPER_CASE
- Functions in snake_case

### Example
```c
/* Good */
void keyboard_handler(void) {
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    if (scancode >= MAX_SCANCODE) {
        return;  /* Invalid scancode */
    }
    process_key(scancode);
}

/* Avoid */
void kbh() {
    int s = inb(0x60);
    pk(s);
}
```

## Testing

Before submitting a pull request:

1. **Build the kernel**
   ```bash
   make clean
   make
   ```

2. **Test in QEMU**
   ```bash
   make run
   ```

3. **Verify your changes**
   - Boot the kernel successfully
   - Test the functionality you modified
   - Ensure no regressions in existing features

## Areas for Contribution

We welcome contributions in:

- **Bug Fixes**: Fix issues in existing code
- **Features**: Implement items from the roadmap (see docs/roadmap/roadmap.md)
- **Documentation**: Improve README, add tutorials, write explanations
- **Code Quality**: Refactor code, add error checking, improve style
- **Testing**: Add test cases, validation checks
- **Drivers**: Implement new hardware drivers
- **Memory Management**: Improve paging, add heap allocator
- **Scheduling**: Add process management and task switching

## Documentation

When adding new features:

- Update relevant documentation files
- Add inline comments for complex code
- Update README.md if user-facing changes
- Document any new APIs or interfaces

## Learning Resources

If you're new to OS development:

- [OSDev.org](https://wiki.osdev.org/) - Comprehensive OS development wiki
- [Writing a Simple Operating System from Scratch](https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf) - Great introduction
- [The little book about OS development](https://littleosbook.github.io/) - Another excellent tutorial
- [xv6 Source Code](https://github.com/mit-pdos/xv6-public) - Educational Unix-like OS

## Questions?

If you have questions:
- Open an issue with the "question" label
- Check existing issues for similar questions
- Review the documentation in the `/docs` directory

## License

By contributing to OpenOS, you agree that your contributions will be licensed under the MIT License.

---

Thank you for contributing to OpenOS! Every contribution, no matter how small, helps make this project better for everyone learning OS development.
