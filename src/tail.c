#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define DEFAULT_NUM_LINES 10
#define BUFFER_SIZE 1024

void print_usage(const char *prog_name) {
    fprintf(stderr, "Uso: %s [-n num] [-c num] [-qv] [ficheiro...]\n", prog_name);
    fprintf(stderr, "Opções:\n");
    fprintf(stderr, "  -n num     Imprimir as últimas 'num' linhas em vez das últimas %d linhas.\n", DEFAULT_NUM_LINES);
    fprintf(stderr, "  -c num     Imprimir os últimos 'num' bytes de cada ficheiro.\n");
    fprintf(stderr, "  -q         Nunca imprimir cabeçalhos com os nomes dos ficheiros.\n");
    fprintf(stderr, "  -v         Sempre imprimir cabeçalhos com os nomes dos ficheiros.\n");
    fprintf(stderr, "  --help     Mostrar esta ajuda e sair.\n");
}

void print_help(const char *prog_name) {
    print_usage(prog_name);
    fprintf(stderr, "\nMostrar a última parte dos ficheiros.\n");
    fprintf(stderr, "Sem FICHEIRO, ou quando FICHEIRO é -, ler da entrada padrão.\n");
}

void print_tail_lines(FILE *file, int num_lines) {
    char **buffer = malloc(num_lines * sizeof(char *));
    for (int i = 0; i < num_lines; i++) {
        buffer[i] = NULL;
    }

    int count = 0;
    size_t line_capacity = 0;
    char *line = NULL;

    while (getline(&line, &line_capacity, file) != -1) {
        if (buffer[count % num_lines]) {
            free(buffer[count % num_lines]);
        }
        buffer[count % num_lines] = strdup(line);
        count++;
    }

    int start = count > num_lines ? count % num_lines : 0;
    int lines_to_print = count > num_lines ? num_lines : count;

    for (int i = 0; i < lines_to_print; i++) {
        fputs(buffer[(start + i) % num_lines], stdout);
        free(buffer[(start + i) % num_lines]);
    }

    free(buffer);
    free(line);
}

void print_tail_bytes(FILE *file, int num_bytes) {
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);

    if (file_size < num_bytes) {
        num_bytes = file_size;
    }

    fseek(file, -num_bytes, SEEK_END);
    char buffer[BUFFER_SIZE];
    int bytes_to_read = num_bytes;

    while (bytes_to_read > 0) {
        int chunk_size = bytes_to_read > BUFFER_SIZE ? BUFFER_SIZE : bytes_to_read;
        fread(buffer, 1, chunk_size, file);
        fwrite(buffer, 1, chunk_size, stdout);
        bytes_to_read -= chunk_size;
    }
}

int main(int argc, char *argv[]) {
    int opt;
    int num_lines = -1;
    int num_bytes = -1;
    int quiet = 0;
    int verbose = 0;

    // Verificar a opção --help antes de processar outras opções
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
        // Nenhum ficheiro especificado, ler da entrada padrão
        if (num_bytes != -1) {
            print_tail_bytes(stdin, num_bytes);
        } else {
            print_tail_lines(stdin, num_lines);
        }
    } else {
        for (int i = optind; i < argc; i++) {
            FILE *file = fopen(argv[i], "r");
            if (!file) {
                // Ficheiro não encontrado, tentar criá-lo
                file = fopen(argv[i], "w");
                if (!file) {
                    fprintf(stderr, "%s: Não existe tal ficheiro ou diretório: %s\n", argv[0], argv[i]);
                    continue;
                } else {
                    printf("Ficheiro %s foi criado\n", argv[i]); // Mensagem de criação do ficheiro
                    fclose(file);
                    continue;
                }
            }

            if (!quiet && (verbose || argc - optind > 1)) {
                if (i > optind) {
                    putchar('\n'); // Separar a saída de múltiplos ficheiros com uma linha nova
                }
                printf("==> %s <==\n", argv[i]);
            }

            if (num_bytes != -1) {
                print_tail_bytes(file, num_bytes);
            } else {
                print_tail_lines(file, num_lines);
            }
            fclose(file);
        }
    }

    return EXIT_SUCCESS;
}
