/*
 * OpenOS - Shell Implementation
 */

#include "shell.h"
#include "string.h"
#include "commands.h"
#include "../drivers/console.h"

/* Maximum number of registered commands */
#define MAX_COMMANDS 32

/* Command registry */
static shell_command_t command_table[MAX_COMMANDS];
static int command_count = 0;

/*
 * Initialize the shell
 * Registers all built-in commands
 */
void shell_init(void) {
    command_count = 0;
    
    /* Register built-in commands */
    commands_register_all();
}

/*
 * Register a command
 */
void shell_register_command(const char* name, const char* description, command_handler_t handler) {
    if (command_count >= MAX_COMMANDS) {
        console_write("Error: Maximum number of commands reached\n");
        return;
    }
    
    command_table[command_count].name = name;
    command_table[command_count].description = description;
    command_table[command_count].handler = handler;
    command_count++;
}

/*
 * Parse command line input into argc/argv
 * Returns argc (number of arguments)
 */
static int parse_command(char* input, char** argv, int max_args) {
    int argc = 0;
    char* token = string_tokenize(input, " \t\n\r");
    
    while (token != 0 && argc < max_args) {
        argv[argc] = token;
        argc++;
        token = string_tokenize(0, " \t\n\r");
    }
    
    return argc;
}

/*
 * Execute a command string
 */
void shell_execute(char* input) {
    /* Skip empty input */
    if (input == 0 || *input == '\0') {
        return;
    }
    
    /* Check if input is only whitespace */
    int only_whitespace = 1;
    for (char* p = input; *p != '\0'; p++) {
        if (!is_whitespace(*p)) {
            only_whitespace = 0;
            break;
        }
    }
    
    if (only_whitespace) {
        return;
    }
    
    /* Parse the command into arguments */
    char* argv[SHELL_MAX_ARGS];
    int argc = parse_command(input, argv, SHELL_MAX_ARGS);
    
    if (argc == 0) {
        return;
    }
    
    /* Look up the command in the command table */
    const char* cmd_name = argv[0];
    for (int i = 0; i < command_count; i++) {
        if (string_compare(cmd_name, command_table[i].name) == 0) {
            /* Found the command, execute it */
            command_table[i].handler(argc, argv);
            return;
        }
    }
    
    /* Command not found */
    console_write("Command not found: ");
    console_write(cmd_name);
    console_write("\n");
    console_write("Type 'help' for a list of available commands.\n");
}

/*
 * Get all registered commands (used by help command)
 */
const shell_command_t* shell_get_commands(int* count) {
    *count = command_count;
    return command_table;
}
