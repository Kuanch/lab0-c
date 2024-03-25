#pragma once

typedef struct {
    int score, move;
} move_t;

void negamax_init();
int negamax_predict(char *table, char *player);
