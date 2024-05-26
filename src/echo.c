#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// Function to print usage information
void print_usage() {
    printf("Usage: ./echo <material> <distance_in_meters> <message>\n");
    printf("Materials:\n");
    printf("  -a: air   (343 m/s)\n");
    printf("  -w: water (1480 m/s)\n");
    printf("  -g: glass (4540 m/s)\n");
    printf("  -s: steel (5960 m/s)\n");
}

int main(int argc, char *argv[]) {
    // Check if --help option is provided
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        print_usage();
        return 0;
    }

    // Check if no material and distance provided
    if (argc == 1 || argv[1][0] != '-') {
        // No material and distance provided, concatenate all arguments into message
        if (argc < 2) {
            fprintf(stderr, "Usage: %s <message>\n", argv[0]);
            return 1;
        }

        char *message = (char *)malloc(1); // Allocate memory for an empty string
        message[0] = '\0'; // Ensure the string is null-terminated
        for (int i = 1; i < argc; i++) {
            strcat(message, argv[i]);
            if (i != argc - 1) {
                strcat(message, " "); // Add space between words if not the last word
            }
        }

        while (1) {
            printf("%s\n", message);
            fflush(stdout);
            sleep(2);
        }

        // Free dynamically allocated memory
        free(message);
    }

    // Material and distance provided, print message once with delay
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <material> <distance_in_meters> <message>\n", argv[0]);
        return 1;
    }

    char *material = NULL;
    double speed_of_sound_m_per_sec = 0.0;

    // Parse material
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

    // Parse distance
    double distance = atof(argv[2]);

    char *message = (char *)malloc(1); // Allocate memory for an empty string
    message[0] = '\0'; // Ensure the string is null-terminated
    for (int i = 3; i < argc; i++) { // Start from index 3
        strcat(message, argv[i]);
        if (i != argc - 1) {
            strcat(message, " "); // Add space between words if not the last word
        }
    }


    // Calculate round-trip distance
    double round_trip_distance = 2 * distance;

    // Calculate delay based on round-trip distance and speed of sound
    double delay_seconds = round_trip_distance / speed_of_sound_m_per_sec;

    // Print material and speed of sound
    printf("Material: %s \tSpeed of sound: %.2f m/s \tDelay: %.2f\n", material, speed_of_sound_m_per_sec, delay_seconds);

    sleep(delay_seconds);
    // Print the message once
    printf("%s\n", message);
    free(message);

    return 0;
}
