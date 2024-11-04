#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "HUFFMAN_TREE.h"

extern const unsigned char huffmanTreeNodes[];

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

void Decompress(FILE* inFile, FILE* outFile, long originalSize) {
    int currentIndex = 0;  
    int bitBuffer;
    int bitsLeft = 0;
    long bytesWritten = 0;
    int huffmanTreeSize = sizeof(huffmanTreeNodes) / sizeof(huffmanTreeNodes[0]);

    //printf("Iniciando descompresión. Tamaño original esperado: %ld bytes\n", originalSize);

    while (bytesWritten < originalSize) {
        if (bitsLeft == 0) {
            bitBuffer = fgetc(inFile); 
            bitsLeft = 8;
            //printf("Nuevo byte leído: 0x%X\n", bitBuffer);  // Mostrar el byte leído en hexadecimal
        }

        // Extraer el bit más significativo del buffer
        int bit = (bitBuffer >> 7) & 1;
        bitBuffer <<= 1;
        bitsLeft--;

        //printf("Bit extraído: %d, Current Index antes del movimiento: %d\n", bit, currentIndex);

        // Moverse en el árbol según el bit leído
        if (bit == 0) {
            currentIndex = 2 * currentIndex + 1;  
        } else {
            currentIndex = 2 * currentIndex + 2;  
        }

        //printf("Current Index después del movimiento: %d\n", currentIndex);

        // Verificar si estamos dentro de los límites del árbol
        if (currentIndex >= huffmanTreeSize) {
            //fprintf(stderr, "Error: Índice fuera de rango en el árbol de Huffman\n");
            break;
        }

        // Si encontramos un símbolo (nodo hoja)
        if (huffmanTreeNodes[currentIndex] != '$') {
            //printf("Símbolo encontrado: %c en índice %d\n", huffmanTreeNodes[currentIndex], currentIndex);
            fputc(huffmanTreeNodes[currentIndex], outFile);
            bytesWritten++;
            currentIndex = 0;  // Regresar a la raíz del árbol para el siguiente símbolo
            //printf("Bytes escritos hasta ahora: %ld\n", bytesWritten);
        }
    }

    //printf("Descompresión completada. Total de bytes escritos: %ld\n", bytesWritten);
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
/*
    printf("Contenido de huffmanTreeNodes:\n");
for (int i = 0; i < sizeof(huffmanTreeNodes) / sizeof(huffmanTreeNodes[0]); i++) {
    if (huffmanTreeNodes[i] == '$') {
        printf("Índice %d: Nodo interno\n", i);
    } else {
        printf("Índice %d: Símbolo %c\n", i, huffmanTreeNodes[i]);
    }
}*/

    char decompressedFileName[1024];
    strcpy(decompressedFileName, argv[1]);
    strcat(decompressedFileName, ".txt");

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