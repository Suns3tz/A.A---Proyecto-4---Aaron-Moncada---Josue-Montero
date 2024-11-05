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
            fputc(currentNode->data, outFile);
            bytesWritten++;
            currentNode = root;  
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
