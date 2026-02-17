/*
 * OpenOS - String Utility Functions Implementation
 */

#include "string.h"

/* Static pointer for string_tokenize */
static char* tokenize_last = 0;

/*
 * Get the length of a string
 */
size_t string_length(const char* str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

/*
 * Compare two strings
 * Returns 0 if equal, negative if str1 < str2, positive if str1 > str2
 */
int string_compare(const char* str1, const char* str2) {
    while (*str1 && *str2 && *str1 == *str2) {
        str1++;
        str2++;
    }
    return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}

/*
 * Copy a string from source to destination
 * Returns pointer to destination
 */
char* string_copy(char* dest, const char* src) {
    char* original_dest = dest;
    while (*src != '\0') {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
    return original_dest;
}

/*
 * Check if a character is a whitespace character
 */
int is_whitespace(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

/*
 * Tokenize a string (similar to strtok)
 * Modifies the input string by replacing delimiters with null terminators
 * Returns pointer to next token, or NULL if no more tokens
 */
char* string_tokenize(char* str, const char* delim) {
    /* If str is NULL, continue from last position */
    if (str == 0) {
        str = tokenize_last;
    }
    
    /* If we've reached the end, return NULL */
    if (str == 0 || *str == '\0') {
        return 0;
    }
    
    /* Skip leading delimiters */
    while (*str != '\0') {
        int is_delim = 0;
        for (const char* d = delim; *d != '\0'; d++) {
            if (*str == *d) {
                is_delim = 1;
                break;
            }
        }
        if (!is_delim) {
            break;
        }
        str++;
    }
    
    /* If we've reached the end, return NULL */
    if (*str == '\0') {
        tokenize_last = 0;
        return 0;
    }
    
    /* Mark the start of the token */
    char* token_start = str;
    
    /* Find the end of the token */
    while (*str != '\0') {
        int is_delim = 0;
        for (const char* d = delim; *d != '\0'; d++) {
            if (*str == *d) {
                is_delim = 1;
                break;
            }
        }
        if (is_delim) {
            *str = '\0';
            tokenize_last = str + 1;
            return token_start;
        }
        str++;
    }
    
    /* Reached end of string */
    tokenize_last = 0;
    return token_start;
}
