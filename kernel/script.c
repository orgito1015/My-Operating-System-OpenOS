/*
 * OpenOS - Shell Scripting Implementation
 */

#include "script.h"
#include "console.h"
#include "string.h"
#include "ipc.h"

/* Global script context */
static script_context_t script_ctx;
static int script_initialized = 0;

/* Initialize scripting subsystem */
void script_init(void) {
    if (script_initialized) return;
    
    console_write("SCRIPT: Initializing shell scripting...\n");
    
    /* Initialize all variables */
    for (int i = 0; i < MAX_VARIABLES; i++) {
        script_ctx.variables[i].is_set = 0;
        script_ctx.variables[i].name[0] = '\0';
        script_ctx.variables[i].value[0] = '\0';
    }
    
    script_ctx.in_if_block = 0;
    script_ctx.if_condition_result = 0;
    script_ctx.loop_depth = 0;
    
    /* Set default variables */
    script_set_var("PATH", "/bin:/usr/bin");
    script_set_var("HOME", "/home");
    script_set_var("PS1", "OpenOS> ");
    
    script_initialized = 1;
    console_write("SCRIPT: Shell scripting enabled\n");
}

/* Set a variable */
int script_set_var(const char* name, const char* value) {
    if (!name || !value) return -1;
    
    /* Check if variable exists */
    for (int i = 0; i < MAX_VARIABLES; i++) {
        if (script_ctx.variables[i].is_set && 
            strcmp(script_ctx.variables[i].name, name) == 0) {
            strncpy(script_ctx.variables[i].value, value, MAX_VAR_VALUE - 1);
            script_ctx.variables[i].value[MAX_VAR_VALUE - 1] = '\0';
            return 0;
        }
    }
    
    /* Create new variable */
    for (int i = 0; i < MAX_VARIABLES; i++) {
        if (!script_ctx.variables[i].is_set) {
            strncpy(script_ctx.variables[i].name, name, MAX_VAR_NAME - 1);
            script_ctx.variables[i].name[MAX_VAR_NAME - 1] = '\0';
            strncpy(script_ctx.variables[i].value, value, MAX_VAR_VALUE - 1);
            script_ctx.variables[i].value[MAX_VAR_VALUE - 1] = '\0';
            script_ctx.variables[i].is_set = 1;
            return 0;
        }
    }
    
    return -1; /* No free slots */
}

/* Get a variable value */
const char* script_get_var(const char* name) {
    if (!name) return NULL;
    
    for (int i = 0; i < MAX_VARIABLES; i++) {
        if (script_ctx.variables[i].is_set && 
            strcmp(script_ctx.variables[i].name, name) == 0) {
            return script_ctx.variables[i].value;
        }
    }
    
    return NULL;
}

/* Unset a variable */
void script_unset_var(const char* name) {
    if (!name) return;
    
    for (int i = 0; i < MAX_VARIABLES; i++) {
        if (script_ctx.variables[i].is_set && 
            strcmp(script_ctx.variables[i].name, name) == 0) {
            script_ctx.variables[i].is_set = 0;
            return;
        }
    }
}

/* Simple condition evaluation */
static int evaluate_condition(const char* cond) {
    if (!cond) return 0;
    
    /* Simple true/false evaluation */
    if (strcmp(cond, "true") == 0 || strcmp(cond, "1") == 0) {
        return 1;
    }
    
    if (strcmp(cond, "false") == 0 || strcmp(cond, "0") == 0) {
        return 0;
    }
    
    /* Check for variable existence */
    if (cond[0] == '$') {
        const char* value = script_get_var(cond + 1);
        return (value != NULL);
    }
    
    return 0;
}

/* Parse if statement */
int script_parse_if(const char* condition) {
    if (!condition) return -1;
    
    script_ctx.in_if_block = 1;
    script_ctx.if_condition_result = evaluate_condition(condition);
    
    return script_ctx.if_condition_result;
}

/* Parse while statement */
int script_parse_while(const char* condition) {
    if (!condition) return -1;
    
    return evaluate_condition(condition);
}

/* Parse for statement */
int script_parse_for(const char* statement) {
    if (!statement) return -1;
    
    /* Simple for loop support */
    script_ctx.loop_depth++;
    
    return 0;
}

/* Helper function to trim leading and trailing whitespace */
static void trim_whitespace(char* str, char** start, char** end) {
    /* Trim leading whitespace */
    *start = str;
    while (**start == ' ' || **start == '\t') (*start)++;
    
    /* Trim trailing whitespace */
    *end = *start + strlen(*start) - 1;
    while (*end > *start && (**end == ' ' || **end == '\t' || **end == '\0')) {
        **end = '\0';
        (*end)--;
    }
}

/* Execute a script */
int script_execute(const char* script) {
    if (!script) return -1;
    
    console_write("SCRIPT: Executing script...\n");
    
    /* Simple line-by-line execution */
    char line[256];
    int line_pos = 0;
    
    while (*script) {
        if (*script == '\n' || line_pos >= 255) {
            line[line_pos] = '\0';
            
            /* Parse and execute line */
            if (line_pos > 0) {
                /* Check for variable assignment */
                char* equals = strchr(line, '=');
                if (equals) {
                    *equals = '\0';
                    
                    /* Trim whitespace from variable name and value */
                    char* var_start;
                    char* var_end;
                    trim_whitespace(line, &var_start, &var_end);
                    
                    char* val_start;
                    char* val_end;
                    trim_whitespace(equals + 1, &val_start, &val_end);
                    
                    script_set_var(var_start, val_start);
                }
                /* Check for if statement */
                else if (strncmp(line, "if ", 3) == 0) {
                    script_parse_if(line + 3);
                }
                /* Execute as shell command */
                else {
                    console_write("  > ");
                    console_write(line);
                    console_write("\n");
                }
            }
            
            line_pos = 0;
        } else {
            line[line_pos++] = *script;
        }
        script++;
    }
    
    console_write("SCRIPT: Execution complete\n");
    return 0;
}

/* I/O redirection (stub) */
int script_redirect_output(const char* filename) {
    if (!filename) return -1;
    
    console_write("SCRIPT: Redirecting output to ");
    console_write(filename);
    console_write("\n");
    
    return 0;
}

int script_redirect_input(const char* filename) {
    if (!filename) return -1;
    
    console_write("SCRIPT: Redirecting input from ");
    console_write(filename);
    console_write("\n");
    
    return 0;
}

/* Pipe support */
int script_create_pipe(const char* cmd1, const char* cmd2) {
    if (!cmd1 || !cmd2) return -1;
    
    console_write("SCRIPT: Creating pipe: ");
    console_write(cmd1);
    console_write(" | ");
    console_write(cmd2);
    console_write("\n");
    
    /* Use IPC pipe mechanism */
    pipe_t* pipe = pipe_create(0, 0);
    if (!pipe) return -1;
    
    /* TODO: Execute cmd1, redirect output to pipe, execute cmd2 with input from pipe */
    
    pipe_close(pipe);
    return 0;
}
