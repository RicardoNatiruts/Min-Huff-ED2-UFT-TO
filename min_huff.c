#include "min_huff.h"

// Função para criar um nó da árvore
HuffNode* criar_no(int byte, unsigned int freq) {
    HuffNode *novo_no = (HuffNode*)malloc(sizeof(HuffNode));
    novo_no->byte = byte;
    novo_no->freq = freq;
    novo_no->left = NULL;
    novo_no->right = NULL;
    return novo_no;
}

// Função para criar a estrutura da Min-Heap
MinHeap* criar_min_heap(unsigned int capacity) {
    MinHeap *heap = (MinHeap*)malloc(sizeof(MinHeap));
    heap->size = 0;
    heap->capacity = capacity;
    heap->data = (HuffNode**)malloc(capacity * sizeof(HuffNode*));
    return heap;
}

// Função para reorganizar a Heap de cima para baixo
void min_heapify(MinHeap *heap, int i) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < heap->size && heap->data[left]->freq < heap->data[smallest]->freq)
        smallest = left;

    if (right < heap->size && heap->data[right]->freq < heap->data[smallest]->freq)
        smallest = right;

    if (smallest != i) {
        HuffNode *tmp = heap->data[i];
        heap->data[i] = heap->data[smallest];
        heap->data[smallest] = tmp;
        min_heapify(heap, smallest);
    }
}

// Função que extrai o nó de menor frequência
HuffNode* extract_min(MinHeap *heap) {
    if (heap->size <= 0) return NULL;
    if (heap->size == 1) {
        heap->size--;
        return heap->data[0];
    }

    HuffNode *root = heap->data[0];
    heap->data[0] = heap->data[heap->size - 1];
    heap->size--;
    min_heapify(heap, 0);

    return root;
}

//Função para inserir um novo nó na Heap
void insert_min_heap(MinHeap *heap, HuffNode *node) {
    heap->size++;
    int i = heap->size - 1;

    while (i != 0 && heap->data[(i - 1) / 2]->freq > node->freq) {
        heap->data[i] = heap->data[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    heap->data[i] = node;
}

//Função que constrói a Árvore de Huffman
HuffNode* construir_arvore_huffman(MinHeap *heap) {
    while (heap->size > 1) {
        HuffNode *left = extract_min(heap);
        HuffNode *right = extract_min(heap);

        HuffNode *pai = criar_no(256, left->freq + right->freq);
        pai->left = left;
        pai->right = right;

        insert_min_heap(heap, pai);
    }
    return extract_min(heap);
}