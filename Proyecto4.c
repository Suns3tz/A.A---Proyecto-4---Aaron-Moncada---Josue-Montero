#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "HUFFMAN_CODES.h"

extern const char * huffmanCodes[256]; 

long countC(FILE* archivo){
    fseek(archivo, 0, SEEK_END);
    long size = ftell(archivo);
    fseek(archivo, 0, SEEK_SET); 
    return size;
}

void writeCompressionTable(const char* originalFileName, long originalSize, long compressedSize) {
    double compressionPercentage = 100.0 * (1 - ((double)compressedSize / originalSize));

    
    printf("| %-18s | %-13ld | %-15ld | %7.2f%%   |\n", 
            originalFileName, originalSize, compressedSize, compressionPercentage);
    printf("---------------------------------------------------\n");

   
}


void writeheader3byte(FILE* archivo, long cant){
    unsigned char byte1 = (cant >> 16) & 0xFF;
    unsigned char byte2 = (cant >> 8) & 0xFF;
    unsigned char byte3 =  cant & 0xFF;   
    fputc(byte1, archivo);
    fputc(byte2, archivo);
    fputc(byte3, archivo);
}

void changeExtention(const char* OgName, char* NewName){
    strcpy(NewName, OgName);
    char * extension = strrchr(NewName, '.');
    if (extension != NULL) {
        *extension = '\0';
    }
    strcat(NewName, ".huff");
}

void Compress(FILE* inFile, FILE* outFile) {
    int bitBuffer = 0;  
    int bitCount = 0;   
    int currentByte;
    
    while ((currentByte = fgetc(inFile)) != EOF) {
        const char *huffCode = huffmanCodes[currentByte];

        for (int i = 0; huffCode[i] != '\0'; i++) {
            bitBuffer <<= 1;             
            if (huffCode[i] == '1') {
                bitBuffer |= 1;         
            }
            bitCount++;
            if (bitCount == 8) {
                fputc(bitBuffer, outFile);
                bitBuffer = 0;  
                bitCount = 0;   
            }
        }
    }

    if (bitCount > 0) {
        //bitBuffer <<= (8 - bitCount); 
        fputc(bitBuffer, outFile);
    }
}

int main(int argc, char* argv[]){
    if (argc < 2) {
        printf("Por favor, inserta uno o varios archivos a codificar.\n");
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        FILE *IN = fopen(argv[i], "rb");
        if (IN == NULL) {
            printf("Archivo inexistente: %s\n", argv[i]);
            continue; 
        }

        const char* OgName = argv[i];
        char NewName[1024];
        changeExtention(OgName, NewName);



        FILE *OUT = fopen(NewName, "wb");
        if (OUT == NULL) {
            perror("No se pudo crear el archivo de salida.\n");
            fclose(IN);
            return 0;
        }

        
        long originalSize = countC(IN);
        writeheader3byte(OUT, originalSize);

        rewind(IN);

        Compress(IN, OUT);

        long newSize = countC(OUT);

        

        if ( i == 1){
            printf("---------------------------------------------------\n");
            printf("| File Name         | Original Size | Compressed Size | Compression |\n");
            printf("---------------------------------------------------\n");
        }
        fclose(IN);
        fclose(OUT);
        writeCompressionTable(OgName, originalSize, newSize);
    }

    return 0;
}