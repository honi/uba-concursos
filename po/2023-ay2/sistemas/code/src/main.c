#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define READ 0
#define WRITE 1
#define N 4
#define K 0

void worker(int i, int fd_read, int fd_write, int fd_result) {
    int number, secret;

    if (i == K) {
        // Si somos el hijo especial generamos el número secreto.
        secret = rand() % 42;
        printf("[worker %d] Número secreto: %d\n", i, secret);
    }

    // Leemos del pipe i (antecesor) hasta llegar al EOF.
    while(read(fd_read, &number, sizeof(number)) > 0) {
        if (i == K && number > secret) {
            // Llegamos a la condición de salida. Iniciamos la terminación en cascada de los procesos del anillo.
            printf("[worker %d] Recibí: %d, superamos mi número secreto\n", i, number);
            write(fd_result, &number, sizeof(number));
            break;
        } else {
            // Somos un hijo normal o no llegamos aún a la condición de salida.
            // Le pasamos el próximo número a nuestro sucesor a través del pipe (i+1) % N.
            number++;
            printf("[worker %d] Enviando número: %d\n", i, number);
            write(fd_write, &number, sizeof(number));
        }
    }

    // Muy importante hacer un exit() acá para terminar el proceso. Si no volvemos a main() y van a pasar cosas raras.
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
    srand(time(NULL));

    // Creamos los N pipes.
    int pipes[N][2];
    for (int i = 0; i < N; i++) pipe(pipes[i]);

    // Creamos el pipe utilizado por el hijo K para informar el número final al padre.
    int result_pipe[2];
    pipe(result_pipe);

    // Creamos los N hijos.
    for (int i = 0; i < N; i++) {
        if (fork() == 0) {
            // Cerramos todas las puntas de los pipes del anillo que no vamos a usar.
            for (int j = 0; j < N; j++) {
                if (j != i) close(pipes[j][READ]);
                if (j != (i+1) % N) close(pipes[j][WRITE]);
            }

            // Ningún hijo necesita leer del pipe de resultado.
            close(result_pipe[READ]);
            // Solo el hijo K necesita escribir en el pipe de resultado.
            if (i != K) close(result_pipe[WRITE]);

            // Llamamos a la función que ejecuta las acciones del hijo (worker).
            // Le pasamos únicamente los file descriptors que necesita (y que no cerramos).
            worker(i, pipes[i][READ], pipes[(i+1) % N][WRITE], result_pipe[WRITE]);
        }
    }

    // Enviamos el número inicial al hijo especial K.
    int start_number = rand() % 10;
    printf("Empezando con el número: %d\n", start_number);
    write(pipes[K][WRITE], &start_number, sizeof(start_number));

    // Cerramos todos los pipes del anillo.
    for (int i = 0; i < N; i++) {
        close(pipes[i][READ]);
        close(pipes[i][WRITE]);
    }

    // Bloqueamos esperando al número final.
    int end_number;
    read(result_pipe[READ], &end_number, sizeof(end_number));

    // Esperamos que terminen todos los procesos hijo y evitamos dejar zombies.
    for (int i = 0; i < N; i++) wait(NULL);

    printf("Terminamos con el número: %d\n", end_number);
    exit(EXIT_SUCCESS);
}
