#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "hw4.h"

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    ChessGame game;
    int connfd = 0;
    struct sockaddr_in serv_addr;

    // Connect to the server
    if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(connfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }


    initialize_game(&game);
    display_chessboard(&game);
    while (1) {
        char received[BUFFER_SIZE];
        char message[BUFFER_SIZE];
        int cmd_result;
        int clientValidInput = 0;
        while(clientValidInput == 0) {
            printf("\nEnter a message for the server: ");
            fgets(message, BUFFER_SIZE, stdin);
            message[strlen(message)-1] = '\0';
            cmd_result = send_command(&game, message, connfd, true);
            printf("cmd_result: %d\n", cmd_result);
            if (cmd_result == COMMAND_ERROR || cmd_result == COMMAND_UNKNOWN || cmd_result == COMMAND_SAVE || cmd_result == COMMAND_DISPLAY) {
                continue;
            }
            else if (cmd_result == COMMAND_FORFEIT) {
                printf("[Client] client forfeiting...\n");
                break;
            }
            else {
                clientValidInput = 1;
            }
        }
        if (clientValidInput == 0) {
            break;
        }
        display_chessboard(&game);
        int read_result = read(connfd, received, BUFFER_SIZE);
        if (read_result <= 0) {
            printf("[Client] read() failed.\n");
            break;
        }
        printf("[Client] received from server: %s\n", received);
        cmd_result = receive_command(&game, received, connfd, false);
        display_chessboard(&game);
        if (cmd_result == COMMAND_FORFEIT) {
            printf("[Client] server forfeiting...\n");
            break;
        }
    }
    // Please ensure that the following lines of code execute just before your program terminates.
    // If necessary, copy and paste it to other parts of your code where you terminate your program.
    printf("[Client] shutting down...\n");
    FILE *temp = fopen("./fen.txt", "w");
    char fen[200];
    chessboard_to_fen(fen, &game);
    fprintf(temp, "%s", fen);
    fclose(temp);
    close(connfd);
    return 0;
}
