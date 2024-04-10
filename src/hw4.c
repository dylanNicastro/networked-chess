#include "hw4.h"

void initialize_game(ChessGame *game) {
    (*game).moveCount = 0;
    (*game).capturedCount = 0;
    const char startingfen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w";
    fen_to_chessboard(startingfen, game);
}

void chessboard_to_fen(char fen[], ChessGame *game) {
    int pos = 0;
    for (int i = 0; i < 8; i++) {
        int count = 0;
        for (int j = 0; j < 8; j++) {
            if ((*game).chessboard[i][j] == '.') {
                //printf("Current char is . at [%d][%d], position in string is %d, j=%d\n", i, j, pos, j);
                count++;
            }
            else {
                if (count > 0) {
                    j--;
                    //printf("Printing %d, position in string is %d, j=%d\n", count, pos, j);
                    fen[pos] = count + '0';
                    count = 0;
                    pos++;
                }
                else {
                    //printf("Printing %c from [%d][%d], position in string is %d, j=%d\n", (*game).chessboard[i][j], i, j, pos, j);
                    fen[pos] = (*game).chessboard[i][j];
                    pos++;
                }
            }
        }
        if (count > 0) {
            //printf("Printing %d, position in string is %d\n", count, pos);
            fen[pos] = count + '0';
            pos++;
        }
        if (i != 7) {
            fen[pos] = '/';
            pos++;
        }
    }
    if ((*game).currentPlayer == WHITE_PLAYER) {
        fen[pos] = ' ';
        fen[pos+1] = 'w';
        fen[pos+2] = '\0';
    }
    else if ((*game).currentPlayer == BLACK_PLAYER) {
        fen[pos] = ' ';
        fen[pos+1] = 'b';
        fen[pos+2] = '\0';
    }
}

bool is_valid_pawn_move(char piece, int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
    int row_diff = abs(dest_row - src_row);
    int col_diff = abs(dest_col - src_col);
    if ((isupper(piece) && src_row < dest_row) || (islower(piece) && src_row > dest_row)) {
        return false;
    }
    if ((row_diff == 1 && col_diff > 1) || (row_diff == 2 && col_diff != 0) || row_diff > 2) {
        return false;
    }
    if (row_diff == 1 && col_diff == 0 && (*game).chessboard[dest_row][dest_col] != '.') {
        return false;
    }
    if (row_diff == 1 && col_diff == 1 && (*game).chessboard[dest_row][dest_col] == '.') {
        return false;
    }
    if (isupper(piece)) {
        if (row_diff == 2 && ((*game).chessboard[dest_row][dest_col] != '.' || (*game).chessboard[dest_row+1][dest_col] != '.')) {
            return false;
        }
    }
    else if (islower(piece)) {
        if (row_diff == 2 && ((*game).chessboard[dest_row][dest_col] != '.' || (*game).chessboard[dest_row-1][dest_col] != '.')) {
            return false;
        }
    }
    
    return true;
}

bool is_valid_rook_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
    //printf("Moving from [%d][%d] to [%d][%d]\n", src_row, src_col, dest_row, dest_col);
    int row_diff = abs(dest_row - src_row);
    int col_diff = abs(dest_col - src_col);
    if (dest_row > src_row && col_diff == 0) {
        // moving down
        for (int i = src_row+1; i < dest_row; i++) {
            if ((*game).chessboard[i][src_col] != '.') {
                //printf("%c found at [%d][%d]\n", (*game).chessboard[i][src_col], i, src_col);
                return false;
            }
        }
        return true;
    }
    else if (dest_row < src_row && col_diff == 0) {
        // moving up
        for (int i = src_row-1; i > dest_row; i--) {
            if ((*game).chessboard[i][src_col] != '.') {
                //printf("%c found at [%d][%d]\n", (*game).chessboard[i][src_col], i, src_col);
                return false;
            }
        }
        return true;
    }
    else if (row_diff == 0 && dest_col > src_col) {
        // moving right
        for (int i = src_col+1; i < dest_col; i++) {
            if ((*game).chessboard[src_row][i] != '.') {
                //printf("%c found at [%d][%d]\n", (*game).chessboard[src_row][i], src_row, i);
                return false;
            }
        }
        return true;
    }
    else if (row_diff == 0 && dest_col < src_col) {
        // moving left
        for (int i = src_col-1; i > dest_col; i--) {
            if ((*game).chessboard[src_row][i] != '.') {
                //printf("%c found at [%d][%d]\n", (*game).chessboard[src_row][i], src_row, i);
                return false;
            }
        }
        return true;
    }
    return false;
}

bool is_valid_knight_move(int src_row, int src_col, int dest_row, int dest_col) {
    int row_diff = abs(dest_row - src_row);
    int col_diff = abs(dest_col - src_col);
    if ((row_diff == 2 && col_diff == 1) || (row_diff == 1 && col_diff == 2)) {
        return true;
    }
    return false;
}

bool is_valid_bishop_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
    //printf("Moving from [%d][%d] to [%d][%d]\n", src_row, src_col, dest_row, dest_col);
    int row_diff = abs(dest_row - src_row);
    int col_diff = abs(dest_col - src_col);
    if (col_diff != row_diff) {
        return false;
    }
    // 7,2 to 5,0
    // 5 < 7, 0 < 2
    if (dest_row > src_row && dest_col > src_col) {
        // moving down+right
        for (int i = 1; i < row_diff; i++) {
            if ((*game).chessboard[src_row+i][src_col+i] != '.') {
                return false;
            }
        }
        return true;
    }
    else if (dest_row < src_row && dest_col > src_col) {
        // moving up+right
        for (int i = 1; i < row_diff; i++) {
            if ((*game).chessboard[src_row-i][src_col+i] != '.') {
                return false;
            }
        }
        return true;
    }
    else if (dest_row > src_row && dest_col < src_col) {
        // moving down+left
        for (int i = 1; i < row_diff; i++) {
            if ((*game).chessboard[src_row+i][src_col-i] != '.') {
                return false;
            }
        }
        return true;
    }
    else if (dest_row < src_row && dest_col < src_col) {
        // moving up+left
        for (int i = 1; i < row_diff; i++) {
            if ((*game).chessboard[src_row-i][src_col-i] != '.') {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool is_valid_queen_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
    int row_diff = abs(dest_row - src_row);
    int col_diff = abs(dest_col - src_col);
    if ((row_diff > 0 && col_diff == 0) || (row_diff == 0 && col_diff > 0)) {
        return is_valid_rook_move(src_row, src_col, dest_row, dest_col, game);
    }
    if (row_diff > 0 && col_diff > 0) {
        return is_valid_bishop_move(src_row, src_col, dest_row, dest_col, game);
    }
    return false;
}

bool is_valid_king_move(int src_row, int src_col, int dest_row, int dest_col) {
    int row_diff = abs(dest_row - src_row);
    int col_diff = abs(dest_col - src_col);
    if (row_diff <= 1 && col_diff <= 1) {
        return true;
    }
    return false;
}

bool is_valid_move(char piece, int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
    if (dest_row < 0 || dest_row > 7 || dest_col < 0 || dest_col > 7) {
        return false;
    }
    if (src_row == dest_row && src_col == dest_col) {
        return false;
    }
    if ((*game).chessboard[src_row][src_col] == '.') {
        return false;
    }
    int srcpiece = isupper(piece) != 0 ? WHITE_PLAYER : BLACK_PLAYER;
    int destpiece = isupper((*game).chessboard[dest_row][dest_col]) != 0 ? WHITE_PLAYER : BLACK_PLAYER;
    if (srcpiece == destpiece) {
        return false;
    }
    switch (tolower(piece)) {
        case 'p': return is_valid_pawn_move(piece, src_row, src_col, dest_row, dest_col, game);
        case 'r': return is_valid_rook_move(src_row, src_col, dest_row, dest_col, game);
        case 'n': return is_valid_knight_move(src_row, src_col, dest_row, dest_col);
        case 'b': return is_valid_bishop_move(src_row, src_col, dest_row, dest_col, game);
        case 'q': return is_valid_queen_move(src_row, src_col, dest_row, dest_col, game);
        case 'k': return is_valid_king_move(src_row, src_col, dest_row, dest_col);
        default: break;
    }
    return false;
}

void fen_to_chessboard(const char *fen, ChessGame *game) {
    int pos = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (fen[pos] == '/') {
                continue;
            }
            else if (isdigit(fen[pos])) {
                int amt = fen[pos]-'0';
                for (int k = 0; k < amt; k++) {
                    (*game).chessboard[i][j] = '.';
                    //printf("Printing . at [%d][%d] with char pos %d\n", i, j, pos);
                    j++;
                }
                j--;
                pos++;
            }
            else {
                (*game).chessboard[i][j] = fen[pos];
                //printf("Printing %c at [%d][%d] with char pos %d\n", fen[pos], i, j, pos);
                pos++;
            }
        }
        pos++;
    }
    if (fen[pos] == 'w') {
        (*game).currentPlayer = WHITE_PLAYER;
    }
    else if (fen[pos] == 'b') {
        (*game).currentPlayer = BLACK_PLAYER;
    }
}

int parse_move(const char *move, ChessMove *parsed_move) {
    if ((int)strlen(move) != 4 && (int)strlen(move) != 5) {
        return PARSE_MOVE_INVALID_FORMAT;
    }
    if (move[0] < 'a' || move[0] > 'h' || move[2] < 'a' || move[2] > 'h') {
        return PARSE_MOVE_INVALID_FORMAT;
    }
    if (move[1] < '1' || move[1] > '8' || move[3] < '1' || move[3] > '8') {
        return PARSE_MOVE_OUT_OF_BOUNDS;
    }
    if (strlen(move) == 5) {
        int src_row = move[1] - '0';
        int dest_row = move[3] - '0';
        if ((src_row == 2 && dest_row == 1) == false && (src_row = 7 && dest_row == 8) == false) {
            return PARSE_MOVE_INVALID_DESTINATION;
        }
        else if (move[4] != 'q' && move[4] != 'r' && move[4] != 'b' && move[4] != 'n') {
            return PARSE_MOVE_INVALID_PROMOTION;
        }
        else {
            (*parsed_move).endSquare[2] = move[4];
            (*parsed_move).endSquare[3] = '\0';
        }
    }
    else {

        (*parsed_move).endSquare[2] = '\0';
    }
    (*parsed_move).startSquare[0] = move[0];
    (*parsed_move).startSquare[1] = move[1];
    (*parsed_move).startSquare[2] = '\0';
    (*parsed_move).endSquare[0] = move[2];
    (*parsed_move).endSquare[1] = move[3];
    return 0;
}

int make_move(ChessGame *game, ChessMove *move, bool is_client, bool validate_move) {
    (void)game;
    (void)move;
    (void)is_client;
    (void)validate_move;
    return -999;
}

int send_command(ChessGame *game, const char *message, int socketfd, bool is_client) {
    (void)game;
    (void)message;
    (void)socketfd;
    (void)is_client;
    return -999;
}

int receive_command(ChessGame *game, const char *message, int socketfd, bool is_client) {
    (void)game;
    (void)message;
    (void)socketfd;
    (void)is_client;
    return -999;
}

int save_game(ChessGame *game, const char *username, const char *db_filename) {
    if (strpbrk(username, " ") != NULL) {
        return -1;
    }
    FILE *save;
    save = fopen(db_filename, "a");
    if (save == NULL) {
        return -1;
    }
    char fen[BUFFER_SIZE];
    chessboard_to_fen(fen, game);
    fprintf(save, "%s:%s\n", username, fen);
    return fclose(save);
}

int load_game(ChessGame *game, const char *username, const char *db_filename, int save_number) {
    FILE *load;
    load = fopen(db_filename, "r");
    if (load == NULL) {
        printf("File not found\n");
        return -1;
    }
    char line[BUFFER_SIZE]; 
    int current = 0;
    char currentuser[BUFFER_SIZE];
    char fen[BUFFER_SIZE];
    while (fgets(line, sizeof(line), load) != NULL) {
        sscanf(line, "%[^:]%*c%[^\n]s", currentuser, fen);
        //printf("%s:%s\n", currentuser, fen);
        if (strcmp(currentuser, username) == 0) {
            current++;
            if (current == save_number) {
                break;
            }
        }
    }
    if (current != save_number) {
        printf("current %d != save_number %d\n", current, save_number);
        return -1;
    }
    (*game).moveCount = 0;
    (*game).capturedCount = 0;
    fen_to_chessboard(fen, game);
    return 0;
}

void display_chessboard(ChessGame *game) {
    printf("\nChessboard:\n");
    printf("  a b c d e f g h\n");
    for (int i = 0; i < 8; i++) {
        printf("%d ", 8 - i);
        for (int j = 0; j < 8; j++) {
            printf("%c ", game->chessboard[i][j]);
        }
        printf("%d\n", 8 - i);
    }
    printf("  a b c d e f g h\n");
}
