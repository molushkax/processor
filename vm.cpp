#include "stack.h"

#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int main () {
    char word[100];
    int b = 0;

    for (int i = 0; i < 3; i++) {
        scanf("%s %d", word, &b);
        printf("%s %d\n", word, b);
    }
}
