#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void print_usage() {
    printf("Usage: ./echo <material> <distance_in_meters> <message>\n");
    printf("Materials:\n");
    printf("  -a: air   (343 m/s)\n");
    printf("  -w: water (1480 m/s)\n");
    printf("  -g: glass (4540 m/s)\n");
    printf("  -s: steel (5960 m/s)\n");
}

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        print_usage();
        return 0;
    }

    if (argc == 1 || argv[1][0] != '-') {
        if (argc < 2) {
            fprintf(stderr, "Usage: %s <message>\n", argv[0]);
            return 1;
        }

        char *message = (char *)malloc(1);
        message[0] = '\0';
        for (int i = 1; i < argc; i++) {
            strcat(message, argv[i]);
            if (i != argc - 1) {
                strcat(message, " ");
            }
        }

        while (1) {
            printf("%s\n", message);
            fflush(stdout);
            sleep(2);
        }

        free(message);
    }

    if (argc < 4) {
        fprintf(stderr, "Usage: %s <material> <distance_in_meters> <message>\n", argv[0]);
        return 1;
    }

    char *material = NULL;
    double speed_of_sound_m_per_sec = 0.0;

    switch (argv[1][1]) {
        case 'a':
            material = "air";
            speed_of_sound_m_per_sec = 343.0;
            break;
        case 'w':
            material = "water";
            speed_of_sound_m_per_sec = 1480.0;
            break;
        case 'g':
            material = "glass";
            speed_of_sound_m_per_sec = 4540.0;
            break;
        case 's':
            material = "steel";
            speed_of_sound_m_per_sec = 5960.0;
            break;
        default:
            fprintf(stderr, "Unknown material: %s\n", argv[1]);
            print_usage();
            return 1;
    }

    double distance = atof(argv[2]);

    char *message = (char *)malloc(1);
    message[0] = '\0';
    for (int i = 3; i < argc; i++) {
        strcat(message, argv[i]);
        if (i != argc - 1) {
            strcat(message, " ");
        }
    }

    double round_trip_distance = 2 * distance;
    double delay_seconds = round_trip_distance / speed_of_sound_m_per_sec;

    printf("Material: %s \tSpeed of sound: %.2f m/s \tDelay: %.2f\n", material, speed_of_sound_m_per_sec, delay_seconds);

    sleep(delay_seconds);
    printf("%s\n", message);
    free(message);

    return 0;
}
