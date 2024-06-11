#include <unistd.h>
#include <stdarg.h>

// This function prints a string and updates the length
void    put_string(char *string, int *length)
{
    // If the string is null, set it to "(null)"
    if (!string)
        string = "(null)";
    // Loop through the string and print each character
    while (*string)
        *length += write(1, string++, 1);
}

// This function prints a digit in a specified base and updates the length
void    put_digit(long long int number, int base, int *length)
{
    // Define a string containing hexadecimal characters
    char    *hexadecimal = "0123456789abcdef";

    // If the number is negative, print a '-' sign and make the number positive
    if (number < 0)
    {
        number *= -1;
        *length += write(1, "-", 1);
    }
    // If the number is greater than or equal to the base, recursively call put_digit
    if (number >= base)
        put_digit((number / base), base, length);
    // Print the digit in the specified base
    *length += write(1, &hexadecimal[number % base], 1);
}

// This is our custom printf function
int    ft_printf(const char *format, ...)
{
    // Initialize the length to 0
    int length = 0;

    // Declare a variable argument list
    va_list pointer;
    // Start the variable argument list
    va_start(pointer, format);

    // Loop through the format string
    while (*format)
    {
        // Check if the current character is '%', indicating a format specifier
        if ((*format == '%') && ((*(format + 1) == 's') || (*(format + 1) == 'd') || (*(format + 1) == 'x')))
        {
            format++; // Move to the next character after '%'
            // Check the format specifier and call the appropriate function
            if (*format == 's')
                put_string(va_arg(pointer, char *), &length);
            else if (*format == 'd')
                put_digit((long long int)va_arg(pointer, int), 10, &length);
            else if (*format == 'x')
                put_digit((long long int)va_arg(pointer, unsigned int), 16, &length);
        }
        else
            length += write(1, format, 1); // Print regular characters
        format++; // Move to the next character in the format string
    }
    // End the variable argument list
    return (va_end(pointer), length);
}


========================GET_NEXT_LINE_==============================

#include "get_next_line.h"

// Function to find the first occurrence of a character in a string
char    *ft_strchr(char *s, int c)
{
    while (*s)
    {
        if (*s == (char)c)
            return ((char *)s); // Return the pointer to the character if found
        s++;
    }
    return (NULL); // Return NULL if character not found
}

// Function to calculate the length of a string
size_t    ft_strlen(const char *s)
{
    size_t i = 0;
    
    while (s[i])
        i++;
    return (i); // Return the length of the string
}

// Function to copy a string
void    ft_strcpy(char *dst, const char *src)
{
    while (*src)    
        *dst++ = *src++; // Copy characters from source to destination
    *dst = '\0'; // Add null terminator at the end
}

// Function to duplicate a string
char    *ft_strdup(const char *src)
{
    size_t  len = ft_strlen(src) + 1;
    char    *dst = malloc(len); // Allocate memory for the new string
    
    if (dst == NULL)
        return (NULL); // Return NULL if memory allocation fails
    ft_strcpy(dst, src); // Copy the string
    return (dst); // Return the duplicated string
}

// Function to concatenate two strings
char    *ft_strjoin(char *s1, char const *s2)
{
    size_t  s1_len = ft_strlen(s1);
    size_t  s2_len = ft_strlen(s2);
    char    *join = malloc((s1_len + s2_len + 1)); // Allocate memory for the concatenated string
    
    if (!s1 || !s2 || !join)
        return (NULL); // Return NULL if any of the strings or the join string is NULL
    ft_strcpy(join, s1); // Copy the first string
    ft_strcpy((join + s1_len), s2); // Copy the second string after the first one
    free(s1); // Free the memory allocated for the first string
    return (join); // Return the concatenated string
}

// Function to read a line from a file descriptor
char    *get_next_line(int fd)
{
    static char buf[BUFFER_SIZE + 1]; // Static buffer to store read characters
    char    *line;
    char    *newline;
    int     countread;
    int     to_copy;

    line = ft_strdup(buf); // Initialize line with content from buffer
    while (!(newline = ft_strchr(line, '\n')) && (countread = read(fd, buf, BUFFER_SIZE)))
    {
        buf[countread] = '\0'; // Null-terminate the buffer
        line = ft_strjoin(line, buf); // Concatenate line with buffer content
    }
    if (ft_strlen(line) == 0)
        return (free(line), NULL); // If line is empty, free memory and return NULL
    
    if (newline != NULL)
    {
        to_copy = newline - line + 1; // Calculate length to copy until newline
        ft_strcpy(buf, newline + 1); // Copy characters after newline to buffer
    }
    else
    {
        to_copy = ft_strlen(line); // Copy entire line
        buf[0] = '\0'; // Reset buffer
    }
    line[to_copy] = '\0'; // Null-terminate line
    return (line); // Return the line read from the file
}
