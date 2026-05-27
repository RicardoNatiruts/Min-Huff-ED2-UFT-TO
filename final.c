#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// 1. ESTRUTURAS DE DADOS
// ============================================================================

typedef struct No {
    unsigned char byte;
    unsigned int frequencia;
    struct No *esq, *dir;
} No;

// Estrutura genérica para o Heap (A ser detalhada por você)
typedef struct {
    No **vetor;
    int tamanho;
    int capacidade;
} MinHeap;

// ============================================================================
// 2. PROTÓTIPOS (Min-Heap a ser implementado)
// ============================================================================

MinHeap* criar_min_heap(int capacidade);
void inserir_no_heap(MinHeap *heap, No *no);
No* extrair_min(MinHeap *heap);
int tamanho_heap(MinHeap *heap);
No* criar_no(unsigned char byte, unsigned int frequencia);
No* criar_no_pai(unsigned int frequencia, No *esq, No *dir);

// ============================================================================
// 3. FUNÇÕES DO ALGORITMO DE HUFFMAN
// ============================================================================
No* criar_no(unsigned char byte, unsigned int frequencia) {
    No* novo = (No*)malloc(sizeof(No));
    novo->byte = byte;
    novo->frequencia = frequencia;
    novo->esq = novo->dir = NULL;
    return novo;
}

No* criar_no_pai(unsigned int frequencia, No *esq, No *dir) {
    No* pai = criar_no('$', frequencia); 
    pai->esq = esq;
    pai->dir = dir;
    return pai;
}

MinHeap* criar_min_heap(int capacidade) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    heap->tamanho = 0;
    heap->capacidade = capacidade;
    heap->vetor = (No**)malloc(capacidade * sizeof(No*));
    return heap;
}

int tamanho_heap(MinHeap *heap) {
    return heap->tamanho;
}

void trocar_nos(No** a, No** b) {
    No* temp = *a;
    *a = *b;
    *b = temp;
}

void min_heapify(MinHeap* heap, int indice) {
    int menor = indice;
    int esq = 2 * indice + 1;
    int dir = 2 * indice + 2;

    if (esq < heap->tamanho && heap->vetor[esq]->frequencia < heap->vetor[menor]->frequencia)
        menor = esq;

    if (dir < heap->tamanho && heap->vetor[dir]->frequencia < heap->vetor[menor]->frequencia)
        menor = dir;

    if (menor != indice) {
        trocar_nos(&heap->vetor[menor], &heap->vetor[indice]);
        min_heapify(heap, menor);
    }
}

void inserir_no_heap(MinHeap *heap, No *no) {
    heap->tamanho++;
    int i = heap->tamanho - 1;

    while (i && no->frequencia < heap->vetor[(i - 1) / 2]->frequencia) {
        heap->vetor[i] = heap->vetor[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    heap->vetor[i] = no;
}

No* extrair_min(MinHeap *heap) {
    No* temp = heap->vetor[0];
    heap->vetor[0] = heap->vetor[heap->tamanho - 1];
    heap->tamanho--;
    min_heapify(heap, 0);
    return temp;
}

void gerar_dicionario(No *raiz, char dicionario[256][256], char *codigo_temp, int profundidade) {
    if (raiz == NULL) return;

    if (raiz->esq == NULL && raiz->dir == NULL) {
        codigo_temp[profundidade] = '\0';
        strcpy(dicionario[raiz->byte], codigo_temp);
        return;
    }

    codigo_temp[profundidade] = '0';
    gerar_dicionario(raiz->esq, dicionario, codigo_temp, profundidade + 1);

    codigo_temp[profundidade] = '1';
    gerar_dicionario(raiz->dir, dicionario, codigo_temp, profundidade + 1);
}

int calcular_bytes_unicos(unsigned int frequencias[]) {
    int contagem = 0;
    for (int i = 0; i < 256; i++) {
        if (frequencias[i] > 0) contagem++;
    }
    return contagem;
}

// ============================================================================
// 4. FLUXO PRINCIPAL DE COMPRESSÃO
// ============================================================================

void comprimir(const char *nome_entrada, const char *nome_saida) {
    unsigned int frequencias[256] = {0};
    FILE *arquivo_entrada = fopen(nome_entrada, "rb");
    
    if (!arquivo_entrada) {
        printf("Erro ao abrir o arquivo de entrada.\n");
        return;
    }

    // 4.1 - Frequência
    int byte_lido;
    while ((byte_lido = fgetc(arquivo_entrada)) != EOF) {
        frequencias[byte_lido]++;
    }

    // 4.2 - Construção da Árvore
    MinHeap *heap = criar_min_heap(256);
    for (int i = 0; i < 256; i++) {
        if (frequencias[i] > 0) {
            inserir_no_heap(heap, criar_no((unsigned char)i, frequencias[i]));
        }
    }

    while (tamanho_heap(heap) > 1) {
        No *esq = extrair_min(heap);
        No *dir = extrair_min(heap);
        No *pai = criar_no_pai(esq->frequencia + dir->frequencia, esq, dir);
        inserir_no_heap(heap, pai);
    }
    No *raiz = extrair_min(heap);

    // 4.3 - Dicionário
    char dicionario[256][256] = {0};
    char codigo_temp[256];
    gerar_dicionario(raiz, dicionario, codigo_temp, 0);

    // 4.4 - Gravação do Cabeçalho (.huff)
    FILE *arquivo_saida = fopen(nome_saida, "wb");
    int contagem = calcular_bytes_unicos(frequencias);
    fwrite(&contagem, sizeof(int), 1, arquivo_saida);

    for (int i = 0; i < 256; i++) {
        if (frequencias[i] > 0) {
            unsigned char b = (unsigned char)i;
            fwrite(&b, sizeof(unsigned char), 1, arquivo_saida);
            fwrite(&frequencias[i], sizeof(unsigned int), 1, arquivo_saida);
        }
    }

    long pos_lixo = ftell(arquivo_saida);
    unsigned char lixo_temp = 0;
    fwrite(&lixo_temp, sizeof(unsigned char), 1, arquivo_saida);

    // 4.5 - Gravação do Payload (Bits)
    rewind(arquivo_entrada); // Volta ao início do arquivo original
    unsigned char byte_saida = 0;
    int bits_preenchidos = 0;

    while ((byte_lido = fgetc(arquivo_entrada)) != EOF) {
        char *codigo_binario = dicionario[byte_lido];
        int j = 0;
        
        while (codigo_binario[j] != '\0') {
            byte_saida = (byte_saida << 1) | (codigo_binario[j] == '1' ? 1 : 0);
            bits_preenchidos++;
            
            if (bits_preenchidos == 8) {
                fwrite(&byte_saida, sizeof(unsigned char), 1, arquivo_saida);
                byte_saida = 0;
                bits_preenchidos = 0;
            }
            j++;
        }
    }

    unsigned char bits_lixo = 0;
    if (bits_preenchidos > 0) {
        bits_lixo = 8 - bits_preenchidos;
        byte_saida = byte_saida << bits_lixo;
        fwrite(&byte_saida, sizeof(unsigned char), 1, arquivo_saida);
    }

    // Atualiza o byte de "Bits Lixo" no cabeçalho
    fseek(arquivo_saida, pos_lixo, SEEK_SET);
    fwrite(&bits_lixo, sizeof(unsigned char), 1, arquivo_saida);

    fclose(arquivo_entrada);
    fclose(arquivo_saida);
    printf("Compressao concluida com sucesso!\n");
}

// ============================================================================
// 5. INTERFACE DE LINHA DE COMANDO (CLI)
// ============================================================================

int main(int argc, char *argv[]) {
    // Verifica se a quantidade mínima de argumentos foi passada
    if (argc < 4) {
        printf("Uso: %s <-c ou -d> <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    // Identifica o modo de operação pelos argumentos [cite: 29]
    if (strcmp(argv[1], "-c") == 0) {
        // Modo Compressão [cite: 36]
        printf("Iniciando compressao de %s para %s...\n", argv[2], argv[3]);
        comprimir(argv[2], argv[3]);
    } 
    else if (strcmp(argv[1], "-d") == 0) {
        // Modo Descompressão [cite: 39]
        printf("Iniciando descompressao de %s para %s...\n", argv[2], argv[3]);
        // descompactar(argv[2], argv[3]); // <-- A ser implementado na Parte B
    } 
    else {
        printf("Opcao invalida. Use -c para comprimir ou -d para descomprimir.\n");
    }

    return 0;
}