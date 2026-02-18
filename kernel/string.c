/*
 * OpenOS - String Utility Functions Implementation
 */

#include "string.h"

/*
 * Static pointer for string_tokenize
 * NOTE: This makes string_tokenize non-reentrant. It is not safe for
 * concurrent use or in interrupt handlers. This is acceptable for the
 * single-threaded shell but should be considered if multitasking is added.
 */
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
 * Concatenate two strings
 * Appends src to the end of dest
 * Returns pointer to destination
 */
char* string_concat(char* dest, const char* src) {
    char* original_dest = dest;
    
    /* Find the end of dest */
    while (*dest != '\0') {
        dest++;
    }
    
    /* Copy src to the end of dest */
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

/* Standard string functions */
size_t strlen(const char* str) {
    return string_length(str);
}

int strcmp(const char* str1, const char* str2) {
    return string_compare(str1, str2);
}

char* strcpy(char* dest, const char* src) {
    return string_copy(dest, src);
}

char* strncpy(char* dest, const char* src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

void* memcpy(void* dest, const void* src, size_t n) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

void* memset(void* ptr, int value, size_t num) {
    unsigned char* p = (unsigned char*)ptr;
    for (size_t i = 0; i < num; i++) {
        p[i] = (unsigned char)value;
    }
    return ptr;
}

char* strchr(const char* str, int ch) {
    while (*str) {
        if (*str == (char)ch) {
            return (char*)str;
        }
        str++;
    }
    return NULL;
}

int strncmp(const char* str1, const char* str2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (str1[i] != str2[i]) {
            return (unsigned char)str1[i] - (unsigned char)str2[i];
        }
        if (str1[i] == '\0') {
            return 0;
        }
    }
    return 0;
}

/* Integer to ASCII conversion */
char* itoa(int value, char* str, int base) {
    char* ptr = str;
    char* ptr1 = str;
    char tmp_char;
    int tmp_value;

    if (base < 2 || base > 36) {
        *str = '\0';
        return str;
    }

    do {
        tmp_value = value;
        value /= base;
        int digit_value = tmp_value - value * base;
        if (digit_value < 0) digit_value = -digit_value;  /* Use absolute value for digit */
        *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[digit_value];
    } while (value);

    if (tmp_value < 0 && base == 10) {
        *ptr++ = '-';
    }
    *ptr-- = '\0';

    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
    return str;
}
