#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define DEFAULT_NUM_LINES 10
#define BUFFER_SIZE 1024

void print_usage(const char *prog_name) {
    fprintf(stderr, "Uso: %s [-arg] [num] [ficheiro.txt]\n", prog_name);
    fprintf(stderr, "Opções:\n");
    fprintf(stderr, "  -n num     Imprimir as primeiras 'num' linhas em vez das primeiras %d linhas.\n", DEFAULT_NUM_LINES);
    fprintf(stderr, "  -c num     Imprimir os primeiros 'num' bytes de cada ficheiro.\n");
    fprintf(stderr, "  -q         Nunca imprimir cabeçalhos com os nomes dos ficheiros.\n");
    fprintf(stderr, "  -v         Imprimir cabeçalhos com os nomes dos ficheiros.\n");
    fprintf(stderr, "  --help     Mostrar help e sair.\n");
}

void print_help(const char *prog_name) {
    print_usage(prog_name);
    fprintf(stderr, "\nMostrar a primeira parte dos ficheiros.\n");
    fprintf(stderr, "Sem FICHEIRO, ou quando FICHEIRO é -, ler da entrada padrão.\n");
}

void print_head_lines(FILE *file, int num_lines) {
    char buffer[BUFFER_SIZE];
    int lines_printed = 0;

    while (lines_printed < num_lines && fgets(buffer, BUFFER_SIZE, file)) {
        fputs(buffer, stdout);
        lines_printed++;
    }
}

void print_head_bytes(FILE *file, int num_bytes) {
    char buffer[BUFFER_SIZE];
    int bytes_read = 0;
    int n;

    while (bytes_read < num_bytes && (n = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (bytes_read + n > num_bytes) {
            n = num_bytes - bytes_read;
        }
        fwrite(buffer, 1, n, stdout);
        bytes_read += n;
    }
}

int main(int argc, char *argv[]) {
    int opt;
    int num_lines = -1;
    int num_bytes = -1;
    int quiet = 0;
    int verbose = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_help(argv[0]);
            return EXIT_SUCCESS;
        }
    }

    while ((opt = getopt(argc, argv, "n:c:qv")) != -1) {
        switch (opt) {
            case 'n':
                num_lines = atoi(optarg);
                if (num_lines <= 0) {
                    fprintf(stderr, "Número inválido de linhas: %d\n", num_lines);
                    return EXIT_FAILURE;
                }
                break;
            case 'c':
                num_bytes = atoi(optarg);
                if (num_bytes <= 0) {
                    fprintf(stderr, "Número inválido de bytes: %d\n", num_bytes);
                    return EXIT_FAILURE;
                }
                break;
            case 'q':
                quiet = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    if (num_lines == -1 && num_bytes == -1) {
        num_lines = DEFAULT_NUM_LINES;
    }

    if (optind == argc) {
        if (num_bytes != -1) {
            print_head_bytes(stdin, num_bytes);
        } else {
            print_head_lines(stdin, num_lines);
        }
    } else {
        for (int i = optind; i < argc; i++) {
            FILE *file = fopen(argv[i], "r");
            if (!file) {
                perror(argv[i]);
                continue;
            }

            if (!quiet && (verbose || argc - optind > 1)) {
                if (i > optind) {
                    putchar('\n');
                }
                printf("==> %s <==\n", argv[i]);
            }

            if (num_bytes != -1) {
                print_head_bytes(file, num_bytes);
            } else {
                print_head_lines(file, num_lines);
            }
            fclose(file);
        }
    }

    return EXIT_SUCCESS;
}
