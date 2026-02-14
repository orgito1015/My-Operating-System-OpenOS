/*
 * OpenOS - Process Management
 * 
 * Provides process structures and management functions.
 * This is a placeholder for future implementation.
 */

#ifndef OPENOS_PROCESS_PROCESS_H
#define OPENOS_PROCESS_PROCESS_H

#include <stdint.h>

/* Process states */
typedef enum {
    PROCESS_STATE_READY,
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_BLOCKED,
    PROCESS_STATE_TERMINATED
} process_state_t;

/* Process control block placeholder */
typedef struct process {
    uint32_t pid;
    process_state_t state;
} process_t;

/* Initialize process management */
void process_init(void);

#endif /* OPENOS_PROCESS_PROCESS_H */
