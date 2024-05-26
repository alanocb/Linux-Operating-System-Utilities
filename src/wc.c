#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

void print_usage(const char *prog_name) {
    fprintf(stderr, "Uso: %s [-l] [-w] [-c] [--help] arquivo\n", prog_name);
    fprintf(stderr, "Opções:\n");
    fprintf(stderr, "  -l         Contar o número de linhas.\n");
    fprintf(stderr, "  -w         Contar o número de palavras.\n");
    fprintf(stderr, "  -c         Contar o número de caracteres.\n");
    fprintf(stderr, "  --help     Exibe esta mensagem de ajuda e sai.\n");
}

void print_help(const char *prog_name) {
    print_usage(prog_name);
    fprintf(stderr, "\nConta linhas, palavras e caracteres em um arquivo.\n");
}

void count_file(const char *file_path, int count_lines, int count_words, int count_chars) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    int lines = 0, words = 0, chars = 0;
    int in_word = 0;
    char c;

    while ((c = fgetc(file)) != EOF) {
        chars++;
        if (c == '\n') {
            lines++;
        }
        if (c == ' ' || c == '\n' || c == '\t') {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            words++;
        }
    }

    fclose(file);

    if (count_lines) {
        printf("Linhas: %d\n", lines);
    }
    if (count_words) {
        printf("Palavras: %d\n", words);
    }
    if (count_chars) {
        printf("Caracteres: %d\n", chars);
    }
}

int main(int argc, char *argv[]) {
    int count_lines = 0;
    int count_words = 0;
    int count_chars = 0;

    struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "lwc", long_options, &option_index)) != -1) {
        if (opt == 0) {
            if (strcmp(long_options[option_index].name, "help") == 0) {
                print_help(argv[0]);
                return EXIT_SUCCESS;
            }
        } else {
            switch (opt) {
                case 'l':
                    count_lines = 1;
                    break;
                case 'w':
                    count_words = 1;
                    break;
                case 'c':
                    count_chars = 1;
                    break;
                default:
                    print_usage(argv[0]);
                    return EXIT_FAILURE;
            }
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Erro: Nenhum arquivo especificado\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    for (int i = optind; i < argc; i++) {
        count_file(argv[i], count_lines, count_words, count_chars);
    }

    return EXIT_SUCCESS;
}
