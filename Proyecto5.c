#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "HUFFMAN_TREE.h"

extern const char * huffmanTreeNodes[];

long readHeader3Byte(FILE* archivo) {
    unsigned char byte1 = fgetc(archivo);
    unsigned char byte2 = fgetc(archivo);
    unsigned char byte3 = fgetc(archivo);
    return (byte1 << 16) | (byte2 << 8) | byte3;
}

void Decompress(FILE* inFile, FILE* outFile, long originalSize) {
    int currentIndex = 0;  
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

        //Aqui es donde se recorre el arbol por lo de los indices
        if (bit == 0) {
            currentIndex = 2 * currentIndex + 1;  
        } else {
            currentIndex = 2 * currentIndex + 2;  

        //Este es el punto donde se escribe el archivo y que esta generando el error del que le hablo
        if (huffmanTreeNodes[currentIndex].symbol != '$') {
            fputc(huffmanTreeNodes[currentIndex].symbol, outFile);
            bytesWritten++;
            currentIndex = 0;  
            }
        }
    }
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

    char decompressedFileName[1024];
    strcpy(decompressedFileName, argv[1]);
    strcat(decompressedFileName, ".decompressed");

    FILE *OUT = fopen(decompressedFileName, "wb");
    if (OUT == NULL) {
        perror("No se pudo crear el archivo descomprimido.\n");
        fclose(IN);
        return 0;
    }

    long originalSize = readHeader3Byte(IN);  // Leer el tamaño original
    Decompress(IN, OUT, originalSize);

    fclose(IN);
    fclose(OUT);

    printf("Descompresión completa: %s\n", decompressedFileName);

    return 0;
}