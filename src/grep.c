#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>

void print_usage(const char *prog_name) {
    fprintf(stderr, "Uso: %s [opcao] <palavra> <ficheiro>\n", prog_name);
    fprintf(stderr, "Opções:\n");
    fprintf(stderr, "  -i          Ignorar maiúsculas e minúsculas\n");
    fprintf(stderr, "  -n          Mostrar números das linhas\n");
    fprintf(stderr, "  -c          Contar o número de ocorrências\n");
    fprintf(stderr, "  -v          Inverter a correspondência\n");
    fprintf(stderr, "  --help      Mostrar esta ajuda e sair\n");
}

void print_help(const char *prog_name) {
    print_usage(prog_name);
    fprintf(stderr, "\nProcurar por <palavra> em cada <ficheiro> ou entrada <palavra>.\n");
    fprintf(stderr, "<palavra>: A palavra, número ou letra a ser procurada.\n");
}

void search_pattern_in_file(const char *filename, const char *pattern, int ignore_case, int show_line_numbers, int count_only, int invert_match) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror(filename);
        return;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int line_number = 1;
    int match_count = 0;

    while ((read = getline(&line, &len, file)) != -1) {
        int found = 0;
        if (ignore_case) {
            char *line_lower = strdup(line);
            char *pattern_lower = strdup(pattern);
            for (int i = 0; line_lower[i]; i++) line_lower[i] = tolower(line_lower[i]);
            for (int i = 0; pattern_lower[i]; i++) pattern_lower[i] = tolower(pattern_lower[i]);
            found = strstr(line_lower, pattern_lower) != NULL;
            free(line_lower);
            free(pattern_lower);
        } else {
            found = strstr(line, pattern) != NULL;
        }

        if (invert_match) found = !found;

        if (found) {
            match_count++;
            if (!count_only) {
                if (show_line_numbers) {
                    printf("%s:%d: %s", filename, line_number, line);
                } else {
                    printf("%s: %s", filename, line);
                }
            }
        }
        line_number++;
    }

    if (count_only) {
        printf("%s: %d\n", filename, match_count);
    }

    free(line);
    fclose(file);
}

int main(int argc, char *argv[]) {
    int opt;
    int ignore_case = 0;
    int show_line_numbers = 0;
    int count_only = 0;
    int invert_match = 0;

    struct option long_options[] = {
        {"help", no_argument, 0, 0},
        {0, 0, 0, 0}
    };
    int option_index = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_help(argv[0]);
            return EXIT_SUCCESS;
        }
    }

    while ((opt = getopt_long(argc, argv, "incv", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'i':
                ignore_case = 1;
                break;
            case 'n':
                show_line_numbers = 1;
                break;
            case 'c':
                count_only = 1;
                break;
            case 'v':
                invert_match = 1;
                break;
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    if (optind >= argc) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    char *pattern = argv[optind];

    for (int i = optind + 1; i < argc; i++) {
        search_pattern_in_file(argv[i], pattern, ignore_case, show_line_numbers, count_only, invert_match);
    }

    return EXIT_SUCCESS;
}
