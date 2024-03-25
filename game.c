#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"

#define LOOKUP(table, i, j, else_value)                         \
    ((i) < 0 || (j) < 0 || (i) > BOARD_SIZE || (j) > BOARD_SIZE \
         ? (else_value)                                         \
         : (table)[GET_INDEX(i, j)])

_Static_assert(BOARD_SIZE <= 26, "Board size must not be greater than 26");
_Static_assert(BOARD_SIZE > 0, "Board size must be greater than 0");
_Static_assert(GOAL <= BOARD_SIZE, "Goal must not be greater than board size");
_Static_assert(GOAL > 0, "Goal must be greater than 0");
_Static_assert(ALLOW_EXCEED == 0 || ALLOW_EXCEED == 1,
               "ALLOW_EXCEED must be a boolean that is 0 or 1");

const line_t lines[4] = {
    {1, 0, 0, 0, BOARD_SIZE - GOAL + 1, BOARD_SIZE},             // ROW
    {0, 1, 0, 0, BOARD_SIZE, BOARD_SIZE - GOAL + 1},             // COL
    {1, 1, 0, 0, BOARD_SIZE - GOAL + 1, BOARD_SIZE - GOAL + 1},  // PRIMARY
    {1, -1, 0, GOAL - 1, BOARD_SIZE - GOAL + 1, BOARD_SIZE},     // SECONDARY
};

static char check_line_segment_win(const char *t, int i, int j, line_t line)
{
    char last = t[GET_INDEX(i, j)];
    if (last == ' ')
        return ' ';
    for (int k = 1; k < GOAL; k++) {
        if (last != t[GET_INDEX(i + k * line.i_shift, j + k * line.j_shift)]) {
            return ' ';
        }
    }
#if !ALLOW_EXCEED
    if (last == LOOKUP(t, i - line.i_shift, j - line.j_shift, ' ') ||
        last ==
            LOOKUP(t, i + GOAL * line.i_shift, j + GOAL * line.j_shift, ' '))
        return ' ';
#endif
    return last;
}

char check_win(char *t)
{
    for (int i_line = 0; i_line < 4; ++i_line) {
        line_t line = lines[i_line];
        for (int i = line.i_lower_bound; i < line.i_upper_bound; ++i) {
            for (int j = line.j_lower_bound; j < line.j_upper_bound; ++j) {
                char win = check_line_segment_win(t, i, j, line);
                if (win != ' ')
                    return win;
            }
        }
    }
    for (int i = 0; i < N_GRIDS; i++)
        if (t[i] == ' ')
            return ' ';
    return 'D';
}

double calculate_win_value(char win, char player)
{
    if (win == player)
        return 1.0;
    if (win == (player ^ 'O' ^ 'X'))
        return 0.0;
    return 0.5;
}

unsigned int calculate_win_value_uint(char win, char player)
{
    if (win == player)
        return 1 << FIXED_POINT_SCALE;
    if (win == (player ^ 'O' ^ 'X'))
        return 0;
    return 1 << (FIXED_POINT_SCALE - 1);
}

inline unsigned int double_to_fixed_point(double value)
{
    return (unsigned int) (value * (1 << FIXED_POINT_SCALE));
}

inline double fixed_point_to_double(unsigned int value)
{
    return (double) value / (1 << FIXED_POINT_SCALE);
}

inline unsigned int fixed_point_divide(unsigned int a, unsigned int b)
{
    return (a << FIXED_POINT_SCALE) / b;
}

unsigned int fixed_point_sqrt(unsigned int x)
{
    unsigned int r = x > 1 ? (x >> 1) : 1;
    unsigned int r_new;
    while (1) {
        if (r == 0)
            return r;
        unsigned int div = fixed_point_divide(x, r);
        r_new = (r + div) >> 1;
        unsigned int diff = r_new - r;
        if (diff < TOLERANCE) {
            return r_new;
        }
        r = r_new;
    }
}

inline unsigned int fixed_point_log(unsigned int x)
{
    return double_to_fixed_point(log(fixed_point_to_double(x)));
}

int *available_moves(char *table)
{
    int *moves = malloc(N_GRIDS * sizeof(int));
    int m = 0;
    for (int i = 0; i < N_GRIDS; i++)
        if (table[i] == ' ')
            moves[m++] = i;
    if (m < N_GRIDS)
        moves[m] = -1;
    // moves = [0, 1, 2, 3, 5, 6, 8, ... -1, null, null, ...]
    return moves;
}

void draw_board(const char *t)
{
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (BOARD_SIZE < 10)
            printf("%2d | ", i + 1);
        else if (BOARD_SIZE >= 10 && BOARD_SIZE < 100)
            printf("%3d | ", i + 1);
        else
            printf("%4d | ", i + 1);

        for (int j = 0; j < BOARD_SIZE; j++) {
            // make background color alter between high-intensity and standard
            if ((i + j) & 1U)
                printf("\x1b[47m");
            else
                printf("\x1b[107m");

            switch (t[GET_INDEX(i, j)]) {
            case 'O':
                printf("\x1b[31m");
                printf(" ○ ");
                printf("\x1b[39m");
                break;
            case 'X':
                printf("\x1b[34m");
                printf(" × ");
                printf("\x1b[39m");
                break;
            default:
                printf("   ");
                break;
            }
            printf("\x1b[49m");
        }
        printf("\n");
    }
    if (BOARD_SIZE >= 10)
        printf("-");
    if (BOARD_SIZE >= 100)
        printf("-");
    printf("---+-");
    for (int i = 0; i < BOARD_SIZE; i++)
        printf("---");
    printf("\n");
    if (BOARD_SIZE >= 10)
        printf(" ");
    if (BOARD_SIZE >= 100)
        printf(" ");
    printf("    ");
    for (int i = 0; i < BOARD_SIZE; i++)
        printf(" %2c", 'A' + i);
    printf("\n");
}
