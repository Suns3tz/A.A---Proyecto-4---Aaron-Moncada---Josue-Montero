#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "HUFFMAN_TREE.h"

extern const unsigned char huffmanTreeNodes[];
extern const int huffmanTreeChildren[][2];

long readHeader3Byte(FILE* archivo) {
    unsigned char byte1 = fgetc(archivo);
    unsigned char byte2 = fgetc(archivo);
    unsigned char byte3 = fgetc(archivo);
    return (byte1 << 16) | (byte2 << 8) | byte3;
}

/*
void Decompress(FILE* inFile, FILE* outFile, long originalSize) {
    int currentIndex = 0;
    int bitBuffer;
    int bitsLeft = 0;
    long bytesWritten = 0;
    int huffmanTreeSize = sizeof(huffmanTreeNodes) / sizeof(huffmanTreeNodes[0]);

    while (bytesWritten < originalSize) {
        if (bitsLeft == 0) {
            bitBuffer = fgetc(inFile);
            bitsLeft = 8;
        }

        int bit = (bitBuffer >> 7) & 1;
        bitBuffer <<= 1;
        bitsLeft--;

        if (bit == 0) {
            currentIndex = 2 * currentIndex + 1;
        } else {
            currentIndex = 2 * currentIndex + 2;
        }

        if (currentIndex >= huffmanTreeSize) {
            fprintf(stderr, "Error: Índice fuera de rango en el árbol de Huffman\n");
            break;
        }

        if (huffmanTreeNodes[currentIndex] != '$') {
            fputc(huffmanTreeNodes[currentIndex], outFile);
            bytesWritten++;
            currentIndex = 0;
        }
    }
}
*/

/*
MinHeapNode* crearNodo(unsigned char data) {
    MinHeapNode* nodo = (MinHeapNode*)malloc(sizeof(MinHeapNode));
    nodo->data = data;
    nodo->left = nodo->right = NULL;
    return nodo;
}

MinHeapNode* reconstruirArbol(const unsigned char* arr, int* index, int size) {
    if (*index >= size) return NULL;

    unsigned char simbolo = arr[*index];
    (*index)++;

    MinHeapNode* nodo = crearNodo(simbolo);
    printf("Nodo creado: %c (index: %d)\n", simbolo, *index - 1);

    if (simbolo == '$') { // Nodo interno
        nodo->left = reconstruirArbol(arr, index, size);
        nodo->right = reconstruirArbol(arr, index, size);
        printf("Nodo interno: %c -> left: %c, right: %c\n", 
               simbolo, 
               nodo->left ? nodo->left->data : 'N', 
               nodo->right ? nodo->right->data : 'N');
    }

    return nodo;
}
*/

MinHeapNode* crearNodo(unsigned char data) {
    MinHeapNode* nodo = (MinHeapNode*)malloc(sizeof(MinHeapNode));
    nodo->data = data;
    nodo->left = nodo->right = NULL;
    return nodo;
}

MinHeapNode* reconstruirArbol(const unsigned char* nodes, const int children[][2], int index) {
    if (index == -1) return NULL; 


    MinHeapNode* nodo = crearNodo(nodes[index]);

    
    nodo->left = reconstruirArbol(nodes, children, children[index][0]);
    nodo->right = reconstruirArbol(nodes, children, children[index][1]);


    return nodo;
}

void Decompress(FILE* inFile, FILE* outFile, long originalSize, MinHeapNode* root) {
    MinHeapNode* raiz = root;
    MinHeapNode* currentNode = root;
    int bitBuffer;
    int bitsLeft = 0;
    long bytesWritten = 0;

    while (bytesWritten < originalSize) {
        if (bitsLeft == 0) {
            bitBuffer = fgetc(inFile); 
            bitsLeft = 8;
            printf("Nuevo byte leído: 0x%X\n", bitBuffer);  
        }

        
        int bit = (bitBuffer >> 7) & 1;
        bitBuffer <<= 1;
        bitsLeft--;

        
        if (bit == 0) {
    if (currentNode->left == NULL) {
        fprintf(stderr, "Error: Nodo izquierdo es NULL en bit 0. Current data: %c\n", currentNode->data);
        return;
    }
    currentNode = currentNode->left;
} else {
    if (currentNode->right == NULL) {
        fprintf(stderr, "Error: Nodo derecho es NULL en bit 1. Current data: %c\n", currentNode->data);
        return;
    }
    currentNode = currentNode->right;
}

        
        if (currentNode->left == NULL && currentNode->right == NULL) {
            printf("Símbolo encontrado: %c\n", currentNode->data);
            fputc(currentNode->data, outFile);
            bytesWritten++;
            currentNode = root;  
            printf("Bytes escritos hasta ahora: %ld\n", bytesWritten);
        }
    }

    printf("Descompresión completada. Total de bytes escritos: %ld\n", bytesWritten);
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Por favor, proporciona un archivo a descomprimir.\n");
        return 0;
    }

    FILE *IN = fopen(argv[1], "rb");
    if (IN == NULL) {
        perror("No se pudo abrir el archivo comprimido.\n");
        return 0;
    }

    
    int index = 0;
    //MinHeapNode* huffmanTreeRoot = reconstruirArbol(huffmanTreeNodes, &index, sizeof(huffmanTreeNodes));
    MinHeapNode* huffmanTreeRoot = reconstruirArbol(huffmanTreeNodes, huffmanTreeChildren, 0);

    char decompressedFileName[1024];
    strcpy(decompressedFileName, argv[1]);
    strcat(decompressedFileName, ".txt");

    FILE *OUT = fopen(decompressedFileName, "wb");
    if (OUT == NULL) {
        perror("No se pudo crear el archivo descomprimido.\n");
        fclose(IN);
        return 0;
    }

    long originalSize = readHeader3Byte(IN);  
    Decompress(IN, OUT, originalSize, huffmanTreeRoot);

    fclose(IN);
    fclose(OUT);

    printf("Descompresión completa: %s\n", decompressedFileName);

    return 0;
}
