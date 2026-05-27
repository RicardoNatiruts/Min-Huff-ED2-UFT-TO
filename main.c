#include "min_huff.h"

#define TAMANHO_TABELA 256  

int main(int argc, char *argv[]) {
    // 1. Validação da Interface de Linha de Comando (CLI) 
    // O comando deve ter 4 partes: ./huffman -c entrada.txt saida.huff 
    if (argc < 4) {
        printf("Uso correto: %s -c <arquivo_origem> <arquivo_destino>\n", argv[0]);
        return 1;
    }

    // Verifica se o modo escolhido foi o de compressão (-c) 
    if (strcmp(argv[1], "-c") != 0) {
        printf("Erro: Por enquanto, apenas a compressao (-c) esta implementada.\n");
        return 1;
    }

    char *nome_arquivo_entrada = argv[2];
    char *nome_arquivo_saida = argv[3]; // Será usado na etapa de gravação do .huff

    // 2. Abertura do Arquivo de Entrada (Ex: Livro Dom Casmurro) 
    // "rb" significa "read binary", essencial para ler qualquer arquivo byte a byte 
    FILE *arquivo = fopen("tarciso.txt", "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo: %s\n", "tarciso.txt");
        return 1;
    }

    // 3. Leitura e Cálculo das Frequências 
    unsigned int frequencia[TAMANHO_TABELA] = {0};
    int byte;
    unsigned int total_bytes = 0;

    printf("Lendo o arquivo '%s'...\n", "tarciso.txt");
    while ((byte = fgetc(arquivo)) != EOF) {
        frequencia[byte]++;
        total_bytes++;
    }
    fclose(arquivo);

    if (total_bytes == 0) {
        printf("O arquivo de entrada esta vazio!\n");
        return 1;
    }
    printf("Sucesso! Total de bytes lidos: %u\n", total_bytes);

    // 4. Inicialização da Min-Heap
    MinHeap *heap = criar_min_heap(512);
    int folhas_inseridas = 0;

    // 5. Criação e Inserção dos Nós Folha
    for (int i = 0; i < TAMANHO_TABELA; i++) {
        if (frequencia[i] > 0) {
            HuffNode *novo_no = criar_no(i, frequencia[i]);
            insert_min_heap(heap, novo_no);
            folhas_inseridas++;
        }
    }
    printf("Quantidade de bytes unicos encontrados: %d\n", folhas_inseridas);

    // 6. Construção da Árvore de Huffman
    HuffNode *raiz_huffman = construir_arvore_huffman(heap);

    // Verificação de Sucesso: A frequência da raiz DEVE ser igual ao total de bytes do arquivo!
    printf("Arvore de Huffman construida com sucesso!\n");
    printf("Frequencia na raiz da arvore: %u\n", raiz_huffman->freq);
    printf("\n--- ARVORE DE HUFFMAN GERADA ---\n\n");
    imprimir_arvore(raiz_huffman, 0);
    printf("\n--------------------------------\n");

    /* * PRÓXIMAS ETAPAS (A implementar):
     * 1. Criar o dicionário (tabela de novos códigos em bits) a partir da raiz.
     * 2. Reabrir o arquivo original.
     * 3. Gravar o cabeçalho e os bits comprimidos no arquivo .huff[cite: 14, 15, 16].
     */

    return 0;
}