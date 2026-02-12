/*
 * OpenOS - Keyboard Driver Implementation
 */

#include "keyboard.h"
#include "pic.h"
#include <stdint.h>
#include <stddef.h>

/* External terminal functions from kernel.c */
extern void terminal_put_char(char c);
extern void terminal_backspace(void);

/* US QWERTY scan code to ASCII translation table (Set 1) */
static const char scancode_to_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, /* Left Ctrl */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, /* Left Shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 
    0, /* Right Shift */
    '*',
    0, /* Alt */
    ' ', /* Space */
    0, /* Caps Lock */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* F1-F10 */
    0, /* Num Lock */
    0, /* Scroll Lock */
    0, /* Home */
    0, /* Up Arrow */
    0, /* Page Up */
    '-',
    0, /* Left Arrow */
    0,
    0, /* Right Arrow */
    '+',
    0, /* End */
    0, /* Down Arrow */
    0, /* Page Down */
    0, /* Insert */
    0, /* Delete */
    0, 0, 0,
    0, 0, /* F11, F12 */
    0, /* Rest are undefined */
};

/* Shifted characters */
static const char scancode_to_ascii_shift[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, /* Left Ctrl */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, /* Left Shift */
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 
    0, /* Right Shift */
    '*',
    0, /* Alt */
    ' ', /* Space */
    0, /* Caps Lock */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* F1-F10 */
    0, /* Num Lock */
    0, /* Scroll Lock */
    0, /* Home */
    0, /* Up Arrow */
    0, /* Page Up */
    '-',
    0, /* Left Arrow */
    0,
    0, /* Right Arrow */
    '+',
    0, /* End */
    0, /* Down Arrow */
    0, /* Page Down */
    0, /* Insert */
    0, /* Delete */
    0, 0, 0,
    0, 0, /* F11, F12 */
    0, /* Rest are undefined */
};

/* Keyboard state */
static uint8_t shift_pressed = 0;
static uint8_t caps_lock = 0;

/* Input buffer */
#define INPUT_BUFFER_SIZE 256
static char input_buffer[INPUT_BUFFER_SIZE];
static int input_buffer_pos = 0;
static int line_ready = 0;

/* Initialize keyboard */
void keyboard_init(void) {
    /* Enable keyboard interrupt (IRQ1) */
    uint8_t mask = inb(PIC1_DATA);
    mask &= ~(1 << 1);  /* Clear bit 1 to enable IRQ1 */
    outb(PIC1_DATA, mask);
}

/* Keyboard interrupt handler */
void keyboard_handler(void) {
    /* Read scan code from keyboard */
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    /* Check if it's a break code (key release) */
    if (scancode & 0x80) {
        /* Key released */
        scancode &= 0x7F;  /* Remove break bit */
        
        /* Check for shift key release */
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = 0;
        }
    } else {
        /* Key pressed */
        
        /* Check for shift key press */
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = 1;
        }
        /* Check for caps lock */
        else if (scancode == 0x3A) {
            caps_lock = !caps_lock;
        }
        /* Regular key press */
        else {
            char ascii = 0;
            
            /* Get ASCII character */
            if (shift_pressed) {
                ascii = scancode_to_ascii_shift[scancode];
            } else {
                ascii = scancode_to_ascii[scancode];
            }
            
            /* Apply caps lock for letters (only when shift is not pressed) */
            if (caps_lock && !shift_pressed) {
                if (ascii >= 'a' && ascii <= 'z') {
                    ascii -= 32;  /* Convert to uppercase */
                }
            }
            
            /* Handle special keys */
            if (ascii == '\b') {
                /* Backspace */
                if (input_buffer_pos > 0) {
                    input_buffer_pos--;
                    terminal_backspace();
                }
            } else if (ascii == '\n') {
                /* Enter */
                terminal_put_char('\n');
                input_buffer[input_buffer_pos] = '\0';
                line_ready = 1;
            } else if (ascii != 0) {
                /* Regular character */
                if (input_buffer_pos < INPUT_BUFFER_SIZE - 1) {
                    input_buffer[input_buffer_pos++] = ascii;
                    terminal_put_char(ascii);
                }
            }
        }
    }
    
    /* Send EOI to PIC */
    pic_send_eoi(1);
}

/* Get a line of input (blocking) */
void keyboard_get_line(char* buffer, int max_len) {
    /* Reset buffer */
    input_buffer_pos = 0;
    line_ready = 0;
    
    /* Wait for line to be ready (interrupts must be enabled) */
    while (!line_ready) {
        __asm__ __volatile__("hlt");
    }
    
    /* Copy to output buffer */
    int i;
    for (i = 0; i < input_buffer_pos && i < max_len - 1; i++) {
        buffer[i] = input_buffer[i];
    }
    buffer[i] = '\0';
}
