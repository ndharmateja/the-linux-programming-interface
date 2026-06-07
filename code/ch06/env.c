#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void print_path_folders(char *path_str)
{
    // Skip characters upto and including '='
    char *start = path_str, *end;
    size_t i;
    while (*start != '\0' && *start != '=')
        start++;

    // If we reach the end of the string we can exit
    if (*start == '\0')
        return;

    // At this point p is pointing to an '=' sign, so we can skip it
    start++;

    // Print each folder one by one
    printf("PATH components:\n");
    i = 1;
    while (*start != '\0')
    {
        // Keep incrementing end until we reach a '\0' or ':'
        end = start;
        while (*end != '\0' && *end != ':')
            end++;

        // At this point we need to print path_str[start:end] (python style)
        // Print (end - start) number of characters starting at start
        printf("%ld. %.*s\n", i, (int)(end - start), start);

        // If end is \0 we can exit
        if (*end == '\0')
            return;

        // Otherwise we can update start
        start = end + 1;
        i++;
    }
}

int main(int argc, char *argv[], char *envp[])
{
    char **p;
    for (p = envp; *p != NULL; p++)
        if (strncmp(*p, "PATH=", 5) == 0)
            print_path_folders(*p);
    exit(EXIT_SUCCESS);
}