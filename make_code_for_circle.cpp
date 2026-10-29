#include <stdio.h>
#include <math.h>
#include "vm.h"

#define RED "\033[31m"
#define GREEN_BACKGROUND "\033[42m"
#define YELLOW_BACKGROUND "\033[43m"
#define NO_COLOR "\033[0m"
#define SP "%lg"
#define FATAL_ERROR 67

int main () {
    FILE* circle_file = fopen("code_circle.asm", "w");

    int size = SIZE_OF_SIDE;
    int R = 8;
    int x0 = size/2, y0 = size/2;

    int circle_matrix[size][size];

    for (int x = 0; x < size; x++)
    {
        for (int y = 0; y < size; y++)
        {
            if (pow(x - x0, 2) + pow(y - y0, 2) <= pow(R * 1.1, 2))
            {
                circle_matrix[x][y] = 1;
                printf ("%d ", circle_matrix[x][y]);
                fprintf (circle_file, "PUSH 67\n");
                fprintf (circle_file, "POP [%d]\n", y + x * size);
            }
            else
            {
                circle_matrix[x][y] = 0;
                printf ("  ");
                fprintf (circle_file, "PUSH 00\n");
                fprintf (circle_file, "POP [%d]\n", y + x * size);
            }
        }
        printf ("\n");
    }
    fprintf (circle_file, "DRAW\n");
    fprintf (circle_file, "HLT\n");

    return 0;
}
