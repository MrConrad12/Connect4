/* 
   Connect Four Game in C

   This program is a text-based implementation of the Connect Four game.
   The game is played on a grid with 7 columns and 6 rows.

   Features:
   - Two players take turns to drop their tokens (O for Player 1 and X for Player 2) in a column.
   - The game checks for a win condition when a player has four tokens in a row, column, or diagonal.
   - It also checks for a draw when all positions on the grid are filled.
   - Players can quit the game at any time.
   - The grid is displayed after each move to keep track of the game progress.
   
   How to Play:
   - Players take turns entering the column number (1 to 7) where they want to drop their token.
   - The program validates the input and checks if the move is legal.
   - If a player wins or the game ends in a draw, the result is displayed.
   - Players can enter 'Q' or 'q' to quit the game at any time.

   Note:
   - The code handles input validation, preventing invalid moves.
   - It tracks and displays the game status, including wins and draws.
   - The grid is cleared and re-initialized for a new game.
   
   Author: [Your Name]
   Date: [Date]
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define P4_COLUMNS (7)
#define P4_ROWS (6)

#define PLAYER1_TOKEN ('O')
#define PLAYER2_TOKEN ('X')

#define ACTION_ERROR (0)
#define ACTION_PLAY (1)
#define ACTION_NEW_INPUT (2)
#define ACTION_QUIT (3)

#define STATUS_OK (0)
#define STATUS_WIN (1)
#define STATUS_DRAW (2)

struct position {
    int column;
    int row;
};

static void display_grid(void);
static void calculate_position(int, struct position *);
static unsigned calculate_token_count_from_to(struct position *, int, int, char);
static unsigned calculate_token_count(struct position *, char);
static int valid_move(int);
static int request_action(int *);
static int is_grid_full(void);
static void initialize_grid(void);
static int valid_position(struct position *);
static int game_status(struct position *pos, char);
static unsigned max(unsigned, unsigned);
static int flush_buffer(FILE *);

static char grid[P4_COLUMNS][P4_ROWS];

static void display_grid(void) {
    int col;
    int row;
    putchar('\n');

    for (col = 1; col <= P4_COLUMNS; ++col)
        printf("  %d ", col);

    putchar('\n');
    putchar('+');

    for (col = 1; col <= P4_COLUMNS; ++col)
        printf("---+");

    putchar('\n');

    for (row = 0; row < P4_ROWS; ++row) {
        putchar('|');
        for (col = 0; col < P4_COLUMNS; ++col)
            if (isalpha(grid[col][row]))
                printf(" %c |", grid[col][row]);
            else
                printf(" %c |", ' ');

        putchar('\n');
        putchar('+');

        for (col = 1; col <= P4_COLUMNS; ++col)
            printf("---+");

        putchar('\n');
    }

    for (col = 1; col <= P4_COLUMNS; col++)
        printf("  %d ", col);

    putchar('\n');
}

static void calculate_position(int move, struct position *pos) {
    // Translate the played move into column and row numbers
    int row;
    pos->column = move;

    for (row = P4_ROWS - 1; row >= 0; --row)
        if (grid[pos->column][row] == ' ') {
            pos->row = row;
            break;
        }
}

static unsigned calculate_token_count_from_to(struct position *pos, int horizontal, int vertical, char token) {
    // Calculate adjacent tokens
    struct position tmp;
    unsigned count = 1;

    tmp.column = pos->column + horizontal;
    tmp.row = pos->row + vertical;

    while (valid_position(&tmp)) {
        if (grid[tmp.column][tmp.row] == token)
            ++count;
        else
            break;

        tmp.column += horizontal;
        tmp.row += vertical;
    }
    return count;
}

static unsigned calculate_token_count(struct position *pos, char token) {
    unsigned max_count;

    max_count = calculate_token_count_from_to(pos, 0, 1, token);

    max_count = max(max_count, calculate_token_count_from_to(pos, 1, 0, token)
        + calculate_token_count_from_to(pos, -1, 0, token) - 1);
    max_count = max(max_count, calculate_token_count_from_to(pos, 1, 1, token)
        + calculate_token_count_from_to(pos, -1, -1, token) - 1);
    max_count = max(max_count, calculate_token_count_from_to(pos, 1, -1, token)
        + calculate_token_count_from_to(pos, -1, 1, token) - 1);

    return max_count;
}

static int valid_move(int col) {
    // Determine if the entered move is valid
    if (col <= 0 || col > P4_COLUMNS || grid[col - 1][0] != ' ')
        return 0;
    return 1;
}

static int request_action(int *move) {
    // Interpret inputs
    char c;
    int result = ACTION_ERROR;

    if (scanf("%d", move) != 1) {
        if (scanf(" %c", &c) != 1) {
            fprintf(stderr, "Error while inputting\n");
            return result;
        }

        switch (c) {
        case 'Q':
        case 'q':
            result = ACTION_QUIT;
            break;
        default:
            result = ACTION_NEW_INPUT;
            break;
        }
    }
    else
        result = ACTION_PLAY;

    if (!flush_buffer(stdin)) {
        fprintf(stderr, "Error while flushing buffer.\n");
        result = ACTION_ERROR;
    }

    return result;
}

static int is_grid_full(void) {
    unsigned col;
    unsigned row;

    for (col = 0; col < P4_COLUMNS; col++)
        for (row = 0; row < P4_ROWS; row++)
            if (grid[col][row] == ' ')
                return 0;
    return 1;
}

static void initialize_grid(void) {
    unsigned col;
    unsigned row;

    for (col = 0; col < P4_COLUMNS; col++)
        for (row = 0; row < P4_ROWS; row++)
            grid[col][row] = ' ';
}

static int valid_position(struct position *pos) {
    // Check if the entered position is within the grid
    int result = 1;
    if (pos->column >= P4_COLUMNS || pos->column < 0)
        result = 0;
    if (pos->row >= P4_ROWS || pos->row < 0)
        result = 0;
    return result;
}

static int game_status(struct position *pos, char token) {
    if (is_grid_full())
        return STATUS_DRAW;
    else if (calculate_token_count(pos, token) >= 4)
        return STATUS_WIN;

    return STATUS_OK;
}

static unsigned max(unsigned a, unsigned b) {
    // Return the larger of the two arguments
    return (a > b) ? a : b;
}

static int flush_buffer(FILE *fp) {
    int c;
    do
        c = fgetc(fp);
    while (c != '\n' && c != EOF);
    return ferror(fp) ? 0 : 1;
}

int main(void) {
    int status;
    char token = PLAYER1_TOKEN;

    initialize_grid();
    display_grid();

    while (1) {
        struct position pos;
        int action;
        int move;

        printf("Play %d: ", (token == PLAYER1_TOKEN) ? 1 : 2);
        action = request_action(&move);

        if (action == ACTION_ERROR)
            return EXIT_FAILURE;
        else if (action == ACTION_QUIT)
            return 0;
        else if (action == ACTION_NEW_INPUT || !valid_move(move)) {
            fprintf(stderr, "You cannot play at this location.\n");
            continue;
        }

        calculate_position(move - 1, &pos);
        grid[pos.column][pos.row] = token;
        display_grid();
       

 status = game_status(&pos, token);

        if (status != STATUS_OK)
            break;

        token = (token == PLAYER1_TOKEN) ? PLAYER2_TOKEN : PLAYER1_TOKEN;
    }

    if (status == STATUS_WIN)
        printf("Player %d wins!\n", (token == PLAYER1_TOKEN) ? 1 : 2);
    else if (status == STATUS_DRAW)
        printf("It's a draw!\n");

    return 0;
}

