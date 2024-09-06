#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MATRIX_SIZE 6
#define HISTORY_SIZE 5
#define Q_SIZE 3

double det_history[HISTORY_SIZE];
int history_count = 0;

void print_matrix(double matrix[MATRIX_SIZE][MATRIX_SIZE]) {
    printf("Получена матрица:\n");
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            printf("%.2f ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void get_minor(double matrix[MATRIX_SIZE][MATRIX_SIZE], double minor[MATRIX_SIZE][MATRIX_SIZE], int row, int col, int n) {
    int i = 0, j = 0;
    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            if (r != row && c != col) {
                minor[i][j++] = matrix[r][c];
                if (j == n - 1) {
                    j = 0;
                    i++;
                }
            }
        }
    }
}

double get_det(double matrix[MATRIX_SIZE][MATRIX_SIZE], int n) {
    if (n == 1) {
        return matrix[0][0];
    }

    double det = 0.0;
    double minor[MATRIX_SIZE][MATRIX_SIZE];
    int s = 1;

    for (int i = 0; i < n; i++) {
        get_minor(matrix, minor, 0, i, n);
        det += s * matrix[0][i] * get_det(minor, n - 1);
        s *= -1;
    }

    return det;
}

int main(void) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addr_len = sizeof(address);
    double matrix[MATRIX_SIZE][MATRIX_SIZE];
    int bytes_received = 0;

    memset(det_history, 0, sizeof(det_history));

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(1);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    if (listen(server_fd, Q_SIZE) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(1);
    }

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addr_len)) < 0) {
            perror("Connection failed");
            continue;
        }

        while (1) { 
            bytes_received = recv(new_socket, matrix, sizeof(matrix), 0);

            if (bytes_received <= 0) {
                printf("Connection closed\n");
                break;
            }

            if (bytes_received < sizeof(matrix)) {
                continue;
            }

            double det = get_det(matrix, MATRIX_SIZE);
            print_matrix(matrix);

            if (history_count < HISTORY_SIZE) {
                det_history[history_count++] = det;
            } else {
                for (int i = 1; i < HISTORY_SIZE; ++i) {
                    det_history[i - 1] = det_history[i];
                }
                det_history[HISTORY_SIZE - 1] = det;
            }

            printf("Det. = %.2f\n", det);
            if (history_count == HISTORY_SIZE) {
                double avg_det = 0;
                for (int i = 0; i < HISTORY_SIZE; ++i) {
                    avg_det += det_history[i];
                }
                avg_det /= HISTORY_SIZE;
                printf("Avg. det = %.2f\n", avg_det);
                printf("Del. det = %.2f\n", det_history[0]);
            } else {
                printf("Avg. det = N/A\n");
                printf("Del. det = N/A\n");
            }
        }

        close(new_socket);
    }

    close(server_fd);
    return 0;
}




