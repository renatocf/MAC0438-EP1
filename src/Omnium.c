/* Standard libraries */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int distance, n_ciclists, uniforme;
    
    if (argc != 4) {
        fprintf(stderr, "USAGE: %s d n [v|u]\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    distance   = atoi(argv[1]);
    n_ciclists = atoi(argv[2]);
    
    if (argv[3][1] != '\0') {
        fprintf(stderr, "3rd argument must be 'u' or 'v'\n");
        return EXIT_FAILURE;
    }
    
    switch (argv[3][0]) {
        case 'u': uniforme = 1; break;
        case 'v': uniforme = 0; break;
        default:
            fprintf(stderr, "3rd argument must be 'u' or 'v'\n");
            return EXIT_FAILURE;
    }
    
    printf("d:%d n:%d uniforme:%d\n", distance, n_ciclists, uniforme);
    
    return EXIT_SUCCESS;
}
