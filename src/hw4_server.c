#include "hw4.h"

int main() {
    int listenfd, connfd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set options to reuse the IP address and IP port if either is already in use
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))");
        return -1;
    }
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))");
        return -1;
    }

    // Bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(listenfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(listenfd, 1) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    INFO("Server listening on port %d", PORT);
    // Accept incoming connection
    if ((connfd = accept(listenfd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    INFO("Server accepted connection");

    ChessGame game;
    initialize_game(&game);
    display_chessboard(&game);
    while (1) {
        char received[BUFFER_SIZE];
        int read_result = read(connfd, received, BUFFER_SIZE);
        if (read_result <= 0) {
            printf("[Server] read() failed.\n");
            break;
        }
        int cmd_result = receive_command(&game, received, connfd, true);
        if (cmd_result == COMMAND_FORFEIT) {
            printf("[Server] client forfeiting...\n");
            printf("[Server] shutting down...\n");
            close(connfd);
            close(listenfd);
            return 0;
            break;
        }
        display_chessboard(&game);
        int serverValidInput = 0;
        char message[BUFFER_SIZE];
        while(serverValidInput == 0) {
            printf("\nEnter a message for the client: ");
            fgets(message, BUFFER_SIZE, stdin);
            message[strlen(message)-1] = '\0';
            cmd_result = send_command(&game, message, connfd, false);
            if (cmd_result == COMMAND_ERROR || cmd_result == COMMAND_UNKNOWN || cmd_result == COMMAND_DISPLAY) {
                continue;
            }
            else if (cmd_result == COMMAND_FORFEIT) {
                printf("[Server] server forfeiting...\n");
                break;
            }
            else {
                serverValidInput = 1;
            }
        }
        if (serverValidInput == 0) {
            break;
        }
        display_chessboard(&game);
    }
    printf("[Server] shutting down...\n");
    close(connfd);
    close(listenfd);
    return 0;
}