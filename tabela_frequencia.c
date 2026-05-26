#include <stdio.h>
#include <stdlib.h>

#define TAMANHO_TABELA 256

int main() {
    unsigned int frequencia[TAMANHO_TABELA] = {0}; 
    
    FILE *arquivo = fopen("casmurro.txt", "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return 1;
    }

    int byte;
    unsigned int total_bytes = 0;

    while ((byte = fgetc(arquivo)) != EOF) {
        frequencia[byte]++;
        total_bytes++;
    }

    fclose(arquivo);

    printf("Total de bytes lidos: %u\n\n", total_bytes);
    printf("%s %s\n", "Byte (DEC)", "Frequencia");

    for (int i = 0; i < TAMANHO_TABELA; i++) {
        if (frequencia[i] > 0) {
            printf("%d %u\n", i, frequencia[i]);
        }
    }

    return 0;
}