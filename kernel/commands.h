/*
 * OpenOS - Built-in Shell Commands
 * 
 * Declarations for all built-in shell commands.
 */

#ifndef OPENOS_KERNEL_COMMANDS_H
#define OPENOS_KERNEL_COMMANDS_H

/* Register all built-in commands */
void commands_register_all(void);

/* Individual command handlers */
void cmd_help(int argc, char** argv);
void cmd_clear(int argc, char** argv);
void cmd_echo(int argc, char** argv);
void cmd_uname(int argc, char** argv);
void cmd_uptime(int argc, char** argv);
void cmd_pwd(int argc, char** argv);
void cmd_ls(int argc, char** argv);
void cmd_cd(int argc, char** argv);
void cmd_cat(int argc, char** argv);
void cmd_reboot(int argc, char** argv);

#endif /* OPENOS_KERNEL_COMMANDS_H */
