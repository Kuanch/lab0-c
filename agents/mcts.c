#include <assert.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "mcts.h"
#include "util.h"

struct node {
    int move;
    char player;
    int n_visits;
    unsigned int score;
    struct node *parent;
    struct node *children[N_GRIDS];
};

static struct node *new_node(int move, char player, struct node *parent)
{
    struct node *node = malloc(sizeof(struct node));
    node->move = move;
    node->player = player;
    node->n_visits = 0;
    node->score = 0;
    node->parent = parent;
    memset(node->children, 0, sizeof(node->children));
    return node;
}

static void free_node(struct node *node)
{
    for (int i = 0; i < N_GRIDS; i++)
        if (node->children[i])
            free_node(node->children[i]);
    free(node);
}

static inline double uct_score(int n_total, int n_visits, double score)
{
    if (n_visits == 0)
        return DBL_MAX;
    double exploitation = score / n_visits;
    double exploration = sqrt(log(n_total) / n_visits);
    return exploitation + EXPLORATION_FACTOR * exploration;
}

static inline unsigned int uct_score_fp(unsigned int n_total,
                                        unsigned int n_visits,
                                        unsigned int score)
{
    if (n_visits == 0)
        return UINT_MAX;
    unsigned int exploitation = fixed_point_divide(score, n_visits);
    unsigned int exploration = fixed_point_sqrt(
        fixed_point_divide(fixed_point_log(n_total), n_visits));
    return exploitation + EXPLORATION_FACTOR * exploration;
}

static struct node *select_move(struct node *node)
{
    struct node *best_node = NULL;
    unsigned int best_score = 0;
    for (int i = 0; i < N_GRIDS; i++) {
        if (!node->children[i])
            continue;
        unsigned int score =
            uct_score_fp(node->n_visits, node->children[i]->n_visits,
                         node->children[i]->score);
        if (score > best_score) {
            best_score = score;
            best_node = node->children[i];
        }
    }
    return best_node;
}

static unsigned int simulate(char *table, char player)
{
    char win;
    char current_player = player;
    char temp_table[N_GRIDS];
    memcpy(temp_table, table, N_GRIDS);
    while (1) {
        int *moves = available_moves(temp_table);
        // if no moves are available, return
        if (moves[0] == -1) {
            free(moves);
            break;
        }
        int n_moves = 0;
        // count the number of available moves
        while (n_moves < N_GRIDS && moves[n_moves] != -1)
            ++n_moves;
        // select a random move
        int move = moves[rand() % n_moves];
        free(moves);
        temp_table[move] = current_player;
        if ((win = check_win(temp_table)) != ' ')
            return calculate_win_value_uint(win, player);
        current_player ^= 'O' ^ 'X';
    }
    return (1 << (FIXED_POINT_SCALE - 1));
}

static void backpropagate(struct node *node, unsigned int score)
{
    while (node) {
        node->n_visits++;
        node->score += score;
        node = node->parent;
        score = 1 - score;
    }
}

static void expand(struct node *node, char *table)
{
    int *moves = available_moves(table);
    int n_moves = 0;
    while (n_moves < N_GRIDS && moves[n_moves] != -1)
        ++n_moves;
    for (int i = 0; i < n_moves; i++) {
        node->children[i] = new_node(moves[i], node->player ^ 'O' ^ 'X', node);
    }
    free(moves);
}

int mcts(char *table, char *player_ptr)
{
    char player = *player_ptr;
    char win;
    struct node *root = new_node(-1, player, NULL);
    for (int i = 0; i < ITERATIONS; i++) {
        struct node *node = root;
        char temp_table[N_GRIDS];
        memcpy(temp_table, table, N_GRIDS);
        while (1) {
            if ((win = check_win(temp_table)) != ' ') {
                unsigned int score =
                    calculate_win_value_uint(win, node->player ^ 'O' ^ 'X');
                backpropagate(node, score);
                break;
            }
            if (node->n_visits == 0) {
                unsigned int score = simulate(temp_table, node->player);
                backpropagate(node, score);
                break;
            }
            if (node->children[0] == NULL)
                expand(node, temp_table);
            node = select_move(node);
            assert(node);
            temp_table[node->move] = node->player ^ 'O' ^ 'X';
        }
    }
    struct node *best_node = NULL;
    int most_visits = -1;
    for (int i = 0; i < N_GRIDS; i++) {
        if (root->children[i] && root->children[i]->n_visits > most_visits) {
            most_visits = root->children[i]->n_visits;
            best_node = root->children[i];
        }
    }
    int best_move = best_node->move;
    free_node(root);
    return best_move;
}
