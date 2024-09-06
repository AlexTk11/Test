#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


#define PORT 8080
#define MATRIX_SIZE 6
#define RAND_SEED 666

void generate_matrix(double matrix[MATRIX_SIZE][MATRIX_SIZE]) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            matrix[i][j] = (double)(rand() % 100) / 10.0;
        }
    }
}


void print_matrix(double matrix[MATRIX_SIZE][MATRIX_SIZE]) {
    printf("Отправляемая матрица:\n");
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            printf("%.2f ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    double matrix[MATRIX_SIZE][MATRIX_SIZE];

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nSocket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        exit(1);
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nconnection failed \n");
        exit(1);
    }

    srand(RAND_SEED);

    while (1) {
        generate_matrix(matrix);
        if(send(sock, matrix, sizeof(matrix), 0) < 0) {
            printf("\nsend failed\n");
            close(sock);
            exit(1);
        }
        printf("Матрица отправлена на сервер\n");
        print_matrix(matrix);
        sleep(5);
    }

    close(sock);
    return 0;
}

