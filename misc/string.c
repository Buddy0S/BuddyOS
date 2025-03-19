#include "string.h"

size_t strlen(const char *str) {
	size_t len = 0;
	
	while (str[len] != '\0') {
		len++;
	}
	
	return len;
}

char *strcpy(char *dest, const char *src) {
	char *dst = dest;
	
	while (*src != '\0') {
		*dst++ = *src++;
	}
	*dst = '\0';

	return dest;
}

char *strncpy(char *dest, const char *src, size_t n) {
	size_t i = 0;

	while (i < n && src[i] != '\0') {
		dest[i] = src[i];
		i++;
	}

	while (i < n) {
		dest[i] = '\0';
		i++;
	}

	return dest;
}

int strcmp(const char *str1, const char *str2) {
	while (*str1 != '\0' && (*str1 == *str2)) {
		str1++;
		str2++;
	}

	return (unsigned char)*str1 - (unsigned char)*str2;
}

int strncmp(const char *str1, const char *str2, size_t n) {
    size_t i = 0;

    while (i < n && str1[i] != '\0' && str1[i] == str2[i]) {
        i++;
    }

    if (i == n) {
        return 0;
    }

    return (unsigned char)str1[i] - (unsigned char)str2[i];
}
