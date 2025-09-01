#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// function to generate a random string of length `size`
void rand_string(char *s, size_t size)
{
    static const char charset[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    if (size) {
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int)(sizeof charset - 1);
            s[n] = charset[key];
        }
        s[size] = '\0'; // null terminator
    }
}

