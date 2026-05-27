#ifndef MIN_HUFF_H
#define MIN_HUFF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 1. Estrutura do Nó da Árvore de Huffman
typedef struct HuffNode {
    int byte;               
    unsigned int freq;      
    struct HuffNode *left;  
    struct HuffNode *right; 
} HuffNode;

// 2. Estrutura da Fila de Prioridade (Min-Heap)
typedef struct MinHeap {
    unsigned int size;       
    unsigned int capacity;   
    HuffNode **data;         
} MinHeap;

HuffNode* criar_no(int byte, unsigned int freq);
MinHeap* criar_min_heap(unsigned int capacity);

void min_heapify(MinHeap *heap, int i);
void build_min_heap(MinHeap *heap);
HuffNode* extract_min(MinHeap *heap);
void insert_min_heap(MinHeap *heap, HuffNode *node);

HuffNode* construir_arvore_huffman(MinHeap *heap);

#endif // MIN_HUFF_H
