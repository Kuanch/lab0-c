#pragma once

#define BOARD_SIZE 4
#define GOAL 3
#define ALLOW_EXCEED 1
#define N_GRIDS (BOARD_SIZE * BOARD_SIZE)
#define GET_INDEX(i, j) ((i) * (BOARD_SIZE) + (j))
#define GET_COL(x) ((x) % BOARD_SIZE)
#define GET_ROW(x) ((x) / BOARD_SIZE)
#define CLEAR_BIT(x, i) ((x) & ~(1 << (i)))
#define FIXED_POINT_SCALE 8
#define TOLERANCE (1 << (FIXED_POINT_SCALE / 2))
#define LN2 double_to_fixed_point(0.6931471805599453)

#define for_each_empty_grid(i, table) \
    for (int i = 0; i < N_GRIDS; i++) \
        if (table[i] == ' ')

typedef struct {
    int i_shift, j_shift;
    int i_lower_bound, j_lower_bound, i_upper_bound, j_upper_bound;
} line_t;

extern const line_t lines[4];

int *available_moves(char *table);
char check_win(char *t);
double calculate_win_value(char win, char player);
unsigned int calculate_win_value_uint(char win, char player);
unsigned int double_to_fixed_point(double value);
double fixed_point_to_double(unsigned int value);
unsigned int fixed_point_divide(unsigned int a, unsigned int b);
unsigned int fixed_point_sqrt(unsigned int x);
unsigned int fixed_point_log(unsigned int x);
void draw_board(const char *t);
