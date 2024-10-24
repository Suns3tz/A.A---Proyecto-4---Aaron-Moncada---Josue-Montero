#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ASCIIcount[255] = {0};

void countfreq(FILE *RFROM) { //Cuenta todas las frecuencias 
    char buffer[1024]; 
    size_t bytes;
    
    while ((bytes = fread(buffer, 1, sizeof(buffer), RFROM)) > 0) { //lee el documento 
        for(size_t i = 0; i < bytes; i++) {
            ASCIIcount[(unsigned char)buffer[i]]++; //Asigna al valor ascii una suma de frecuencia
        }
    }
}

void tablafreq(FILE* Freq){ //Hace la tabla

    fprintf(Freq, "    Hex value   |   Symbol   |  Frequency \n");
    fprintf(Freq, "---------------------------------------\n");

    for(int i = 0; i < 256; i++){
        fprintf( Freq, "    0x%02X        ", i );

        if( i >= 32 && i <= 126){
            fprintf(Freq, "|     '%c'    ", i);
        } else{
            fprintf(Freq,  "|            ", i);
        } 

        fprintf( Freq, "|     %d     \n", ASCIIcount[i]);
    }
}

void extraerfreq(FILE* Freq) { //Extrae las frecuencias guardadas en la tabla de frequencias
    char linea[100];
    int valor_hex, frecuencia;
    char simbolo;

    fgets(linea, sizeof(linea), Freq);
    fgets(linea, sizeof(linea), Freq);

    fgets(linea, sizeof(linea), Freq);
    sscanf(linea, " %x | | %d", &valor_hex, &frecuencia);
    ASCIIcount[0] = frecuencia;

    while (fgets(linea, sizeof(linea), Freq)) {
        if (sscanf(linea, " 0x%02X | '%c' | %d", &valor_hex, &simbolo, &frecuencia) == 3) {
            ASCIIcount[valor_hex] = frecuencia;
        } else if (sscanf(linea, " 0x%02X | | %d", &valor_hex, &frecuencia) == 2) {
            ASCIIcount[valor_hex] = frecuencia;
        }
    }
}



void printFreq(FILE* freq){
    if (freq == NULL){
        perror("Error opening frequency file. 3");
        return;
    }

    char buffer[256];

    printf("\n--- Character Frequencies ---\n");
    while (fgets(buffer, sizeof(buffer), freq) != NULL) { //imprime todo el documento hasta que el buffer sea nulo.
        printf("%s", buffer);
    }
    
}

int main(int argc, char **argv) {

    int esCreado = 0;

    char bufferCHECK[44];
    FILE *freq;

    if (argc < 2) {
        FILE *OUT = fopen(argv[1], "rb");
        if (OUT == NULL) { // lo crea si no existe
            perror("Archivo de Frecuencia inexistente");
            return 0;
        } else {
            rewind(OUT);
            fgets(bufferCHECK, sizeof(bufferCHECK), OUT); // se fija si es de frecuencias

            if (strncmp(bufferCHECK, "    Hex value   |   Symbol   |  Frequency \n", 44) == 0) {
                printFreq(OUT);
                fclose(OUT);
                return 0;
            } else {
                perror("Archivo seleccionado no es de frecuencia");
                fclose(OUT); // si no es de frecuencias solo lo cierra y mas adelante lo crea con el nombre del primer argumento
                return 0;
            }
        }
    }

    // se fija si es nulo
    FILE *OUT = fopen(argv[1], "rb");
    if (OUT == NULL) { // lo crea si no existe
        perror("Error opening file 1");
        OUT = fopen(argv[1], "wb");
        tablafreq(OUT);
        fclose(OUT);
    } else {
        rewind(OUT);
        fgets(bufferCHECK, sizeof(bufferCHECK), OUT); // se fija si es de frecuencias

        if (strncmp(bufferCHECK, "    Hex value   |   Symbol   |  Frequency \n", 44) == 0) {
            extraerfreq(OUT);
            fclose(OUT);
        } else {
            fclose(OUT); // si no es de frecuencias solo lo cierra y mas adelante lo crea con el nombre del primer argumento
        }
    }


    for (int i = 2; i < argc; i++) {
        FILE *input = fopen(argv[i], "rb");
        if (input == NULL) {
            perror("Error opening file 2");
            return 0;
        }

        countfreq(input);
        fclose(input);

        
    }

    freq = fopen(argv[1], "wb"); 

    tablafreq(freq);
    fclose(freq);

    freq = fopen(argv[1], "rb");
    if (freq == NULL) {
        perror("Error opening frequency");
        return 0;
    }

    

    printFreq(freq);
    fclose(freq);

    return 0;
}


/*int main(int argc, char **argv) {

    char bufferCHECK[44];
    FILE *freq;

    FILE *OUT = fopen(argv[1], "rb");
    if (OUT == NULL) {//abre el documento que este en la primera posicion
        perror("Error opening frequency file");
        OUT =  fopen("frequencyfilegenerated.txt", "wb");
        tablafreq(OUT);
    }
    
    rewind(OUT);
    fgets(bufferCHECK, sizeof(bufferCHECK), OUT); //Esto funciona para saber si es una archivo de frecuencias

    if (argc == 2 && strncmp(bufferCHECK, "    Hex value   |   Symbol   |  Frequency \n",44) == 0){
        printFreq(OUT);
        return 0;
    }
    else if(argc == 2 && strncmp(bufferCHECK, "    Hex value   |   Symbol   |  Frequency \n",44) != 0){ //Esto es el caso de que el archivo uno no sea de frecuencia pero solo exista este.
                FILE *input = fopen(argv[1], "rb");
                if (input == NULL) {
                perror("Error opening file");
                return 0;
                }
                countfreq(input);
                fclose(input);
                
                printf("The file opened wasn't a frequency file, a new one was created.");
                OUT = fopen("frequencyfilegenerated.txt", "wb");
                tablafreq(OUT);
                fclose(OUT);
                OUT = fopen("frequencyfilegenerated.txt", "rb");
                printFreq(OUT);
                fclose(OUT);
                return 0;
        }
            
    


    
    rewind(OUT);
   
    

    for(int i = 2; i < argc; i++){//hace el recuento uno por uno de los archivos en lista.
        FILE *input = fopen(argv[i], "rb");
        if (input == NULL) {
        perror("Error opening file");
        return 1;
    }
    freq = fopen("frequencyfilegenerated.txt", "wb");
    extraerfreq(freq);
    fclose(freq);
    countfreq(input);
    fclose(input);
    rewind(OUT);
    } 
    fgets(bufferCHECK, sizeof(bufferCHECK), OUT);
            if (strncmp(bufferCHECK, "    Hex value   |   Symbol   |  Frequency \n",44) == 0){
                fclose(OUT);
                OUT = fopen(argv[1], "wb");
                tablafreq(OUT);
                fclose(OUT);
                FILE *FREQREAD = fopen(argv[1], "rb");
                if (FREQREAD == NULL) {
                perror("Error opening file");
                return 1;
                }
                printFreq(FREQREAD);
                fclose(FREQREAD);  
            }
            else{
                OUT = fopen("frequencyfilegenerated.txt", "wb");
                tablafreq(OUT);
                fclose(OUT);
                FILE *FREQREAD = fopen("frequencyfilegenerated.txt", "rb");
                if (FREQREAD == NULL) {
                perror("Error opening file");
                return 1;
                }
                printFreq(FREQREAD);
                fclose(FREQREAD);  

            }
    

    return 0;
}*/