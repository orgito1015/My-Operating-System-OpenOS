/*
 * OpenOS - Process Management
 *
 * Minimal stub implementation of the process management subsystem.
 * Initializes the process table and sets up internal state so that
 * future work (scheduling, context switching, fork) can be layered on top.
 */

#include "process.h"
#include "../drivers/console.h"

/* Maximum number of concurrent processes supported */
#define PROCESS_TABLE_SIZE 64

/* Process table */
static process_t process_table[PROCESS_TABLE_SIZE];

/* Initialization flag */
static int initialized = 0;

/* Initialize the process management subsystem */
void process_init(void) {
    if (initialized) return;

    /* Zero out every entry in the process table */
    for (int i = 0; i < PROCESS_TABLE_SIZE; i++) {
        process_table[i].pid   = 0;
        process_table[i].state = PROCESS_STATE_TERMINATED;
    }

    initialized = 1;
    console_write("Process: process table initialized (");
    /* Print table size without printf */
    char buf[12];
    int n = PROCESS_TABLE_SIZE;
    int pos = 0;
    do {
        buf[pos++] = '0' + (n % 10);
        n /= 10;
    } while (n > 0);
    /* reverse */
    for (int i = 0, j = pos - 1; i < j; i++, j--) {
        char tmp = buf[i]; buf[i] = buf[j]; buf[j] = tmp;
    }
    buf[pos] = '\0';
    console_write(buf);
    console_write(" slots)\n");
}
