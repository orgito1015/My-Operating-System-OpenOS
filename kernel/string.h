/*
 * OpenOS - String Utility Functions
 * 
 * Basic string manipulation functions for kernel use.
 */

#ifndef OPENOS_KERNEL_STRING_H
#define OPENOS_KERNEL_STRING_H

#include <stddef.h>

/* Get the length of a string */
size_t string_length(const char* str);

/* Compare two strings (returns 0 if equal, non-zero otherwise) */
int string_compare(const char* str1, const char* str2);

/* Copy a string from source to destination */
char* string_copy(char* dest, const char* src);

/* Tokenize a string (similar to strtok) */
char* string_tokenize(char* str, const char* delim);

/* Check if a character is a whitespace character */
int is_whitespace(char c);

#endif /* OPENOS_KERNEL_STRING_H */
