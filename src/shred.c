#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#define DEFAULT_OVERWRITES 3
#define BUFFER_SIZE 4096

void print_usage(const char *prog_name) {
    fprintf(stderr, "Uso: %s [-n num] [-z] [-u] [--help] arquivo\n", prog_name);
    fprintf(stderr, "Opções:\n");
    fprintf(stderr, "  -n num     Número de vezes para sobrescrever o arquivo (padrão é %d).\n", DEFAULT_OVERWRITES);
    fprintf(stderr, "  -z         Adiciona uma sobrescrita final com zeros.\n");
    fprintf(stderr, "  -u         Remove o arquivo após a sobrescrita.\n");
    fprintf(stderr, "  --help     Exibe esta mensagem de ajuda e sai.\n");
}

void print_help(const char *prog_name) {
    print_usage(prog_name);
    fprintf(stderr, "\nSobrescreve o conteúdo de um arquivo para dificultar a recuperação de dados.\n");
}

void overwrite_file(const char *file_path, int num_overwrites, int add_final_zeros, int remove_file) {
    int fd = open(file_path, O_WRONLY);
    if (fd == -1) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    struct stat file_stat;
    if (fstat(fd, &file_stat) == -1) {
        perror("Erro ao obter informações do arquivo");
        close(fd);
        return;
    }

    off_t file_size = file_stat.st_size;
    char buffer[BUFFER_SIZE];

    for (int i = 0; i < num_overwrites; i++) {
        for (off_t j = 0; j < file_size; j += BUFFER_SIZE) {
            size_t bytes_to_write = (file_size - j) > BUFFER_SIZE ? BUFFER_SIZE : (file_size - j);
            for (size_t k = 0; k < bytes_to_write; k++) {
                buffer[k] = rand() % 256;
            }
            if (write(fd, buffer, bytes_to_write) != bytes_to_write) {
                perror("Erro ao sobrescrever o arquivo");
                close(fd);
                return;
            }
        }
        lseek(fd, 0, SEEK_SET);
    }

    if (add_final_zeros) {
        memset(buffer, 0, BUFFER_SIZE);
        for (off_t j = 0; j < file_size; j += BUFFER_SIZE) {
            size_t bytes_to_write = (file_size - j) > BUFFER_SIZE ? BUFFER_SIZE : (file_size - j);
            if (write(fd, buffer, bytes_to_write) != bytes_to_write) {
                perror("Erro ao sobrescrever com zeros");
                close(fd);
                return;
            }
        }
    }

    close(fd);

    if (remove_file) {
        if (unlink(file_path) == -1) {
            perror("Erro ao remover o arquivo");
        } else {
            printf("Arquivo %s foi removido\n", file_path);
        }
    }
}

int main(int argc, char *argv[]) {
    int num_overwrites = DEFAULT_OVERWRITES;
    int add_final_zeros = 0;
    int remove_file = 0;

    struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "n:zu", long_options, &option_index)) != -1) {
        if (opt == 0) {
            if (strcmp(long_options[option_index].name, "help") == 0) {
                print_help(argv[0]);
                return EXIT_SUCCESS;
            }
        } else {
            switch (opt) {
                case 'n':
                    num_overwrites = atoi(optarg);
                    if (num_overwrites <= 0) {
                        fprintf(stderr, "Número inválido de sobrescritas: %d\n", num_overwrites);
                        return EXIT_FAILURE;
                    }
                    break;
                case 'z':
                    add_final_zeros = 1;
                    break;
                case 'u':
                    remove_file = 1;
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
        overwrite_file(argv[i], num_overwrites, add_final_zeros, remove_file);
    }

    return EXIT_SUCCESS;
}
