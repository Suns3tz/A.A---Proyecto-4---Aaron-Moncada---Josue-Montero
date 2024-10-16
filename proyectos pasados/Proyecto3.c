#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 100
#define DEFAULT_MAX_SIZE 1024

unsigned char simbolos[256] = {0};
int ASCIIcount[256] = {0};
int FreqSum = 0;

void printSim() {
    for (int i = 0; i < 256; ++i) 
        printf("%d \n", simbolos[i]);
        printf("\n");
  
    printf("\n"); 
}

typedef struct MinHeapNode {
    unsigned char data; 
    int frequency;
    struct MinHeapNode* left;
    struct MinHeapNode* right;
} MinHeapNode;

typedef struct MinHeap {
    MinHeapNode** elements;
    unsigned size;
    unsigned capacity;
} MinHeap;

MinHeapNode* newNode(unsigned char data, int freq) {
    MinHeapNode* temp = (MinHeapNode*)malloc(sizeof(MinHeapNode));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->frequency = freq;
    return temp;
}

MinHeap* createMinHeap(unsigned capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->elements = (MinHeapNode**)malloc(capacity * sizeof(MinHeapNode*));
    return minHeap;
}

void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

void siftUp(MinHeap* minHeap, int pos) {
    while (pos != 0 && minHeap->elements[pos]->frequency < minHeap->elements[(pos - 1) / 2]->frequency) {
        swapMinHeapNode(&minHeap->elements[pos], &minHeap->elements[(pos - 1) / 2]);
        pos = (pos - 1) / 2;
    }
}

void siftDown(MinHeap* minHeap, int pos) {
    while (2 * pos + 1 < minHeap->size) {
        int smallest = 2 * pos + 1;
        if (smallest + 1 < minHeap->size && minHeap->elements[smallest + 1]->frequency < minHeap->elements[smallest]->frequency) {
            smallest++;
        }
        if (minHeap->elements[pos]->frequency <= minHeap->elements[smallest]->frequency) {
            break;
        }
        swapMinHeapNode(&minHeap->elements[pos], &minHeap->elements[smallest]);
        pos = smallest;
    }
}

int isSizeOne(MinHeap* minHeap) {
    return (minHeap->size == 1);
}

MinHeapNode* extractMin(MinHeap* minHeap) {
    MinHeapNode* temp = minHeap->elements[0];
    minHeap->elements[0] = minHeap->elements[minHeap->size - 1];
    minHeap->size--;
    siftDown(minHeap, 0);
    return temp;
}

void insertMinHeap(MinHeap* minHeap, MinHeapNode* minHeapNode) {
    minHeap->size++;
    int i = minHeap->size - 1;
    while (i && minHeapNode->frequency < minHeap->elements[(i - 1) / 2]->frequency) {
        minHeap->elements[i] = minHeap->elements[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->elements[i] = minHeapNode;
}

void buildMinHeap(MinHeap* minHeap) {
    for (int i = (minHeap->size - 1) / 2; i >= 0; --i) {
        siftDown(minHeap, i);
    }
}

int isLeaf(MinHeapNode* root) {
    return !(root->left) && !(root->right);
}

MinHeap* createAndBuildMinHeap(int size) {
    MinHeap* minHeap = createMinHeap(size);
    for (int i = 0; i <= size; i++) {
        minHeap->elements[i] = newNode(simbolos[i], ASCIIcount[i]);  
    }
    minHeap->size = size;
    buildMinHeap(minHeap);
    return minHeap;
}

MinHeapNode* buildHuffmanTree(int size) {
    MinHeapNode *left, *right, *top;
    MinHeap* minHeap = createAndBuildMinHeap(size);
    while (!isSizeOne(minHeap)) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);
        top = newNode('$', left->frequency + right->frequency);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);
}

// Funciones de manejo de archivos
void extraerfreq(FILE* Freq) { // Extrae las frecuencias guardadas en la tabla de frecuencias
    char linea[100];
    int valor_hex, frecuencia;

    fgets(linea, sizeof(linea), Freq);
    fgets(linea, sizeof(linea), Freq);

    while (fgets(linea, sizeof(linea), Freq)) {
    if (sscanf(linea, " 0x%02X | '%c' | %d", &valor_hex, &simbolos[valor_hex], &frecuencia) == 3) {
        ASCIIcount[valor_hex] = frecuencia;
    } else if (sscanf(linea, " 0x%02X | | %d", &valor_hex, &frecuencia) == 2) {
        ASCIIcount[valor_hex] = frecuencia;
    }
}
}

void Sumfreq() {
    for (int i = 0; i < 256; i++) {
        FreqSum += ASCIIcount[i];
    } 
}

void printArr(int arr[], int n, FILE* Huffman) { 
    for (int i = 0; i < n; ++i) 
        fprintf(Huffman, "%d", arr[i]); 
  
    printf("\n"); 
} 

int esPrimera = 1;

void tablaHuffman(FILE* Huffman, MinHeapNode* root, int arr[], int top) { // Hace la tabla

    if (root->left) { 
        arr[top] = 0; 
        tablaHuffman(Huffman, root->left, arr, top + 1); 
    } 
    if (root->right) { 
        arr[top] = 1; 
        tablaHuffman(Huffman, root->right, arr, top + 1); 
    } 
    if (isLeaf(root)) { 
        fprintf(Huffman, "\n");
        if (root->data >= 32 && root->data <= 126) {
            fprintf(Huffman, "0x%02X | %c               = ", root->data, root->data);
        } else {
            fprintf(Huffman, "0x%02X | <non-printable> = ", root->data);
        }
        printArr(arr, top, Huffman); 
    } 
}
int internalCount = 0;

void guardarArbol(FILE* file, MinHeapNode* root) {
    if (root == NULL) return;

    if (isLeaf(root)) {
        fprintf(file, "#define NODE_0x%02X { '0x%02X', %d }\n", root->data, root->data, root->frequency);
    } else {
        fprintf(file, "#define NODE_INTERNAL%d { '$', %d }\n", ++internalCount, root->frequency);
    }

    guardarArbol(file, root->left);
    guardarArbol(file, root->right);
}

void HuffmanCodes(int size, FILE *HUFFMAN, FILE *HuffmanTree) {  
    MinHeapNode* root = buildHuffmanTree(size); 

    printSim();
    // El tamaño de este array está sujeto a la altura del árbol, por lo que hay que calcular más o menos cual sería un número al que nunca llegaría
    int arr[300], top = 0; 
    
    guardarArbol(HuffmanTree, root);
    tablaHuffman(HUFFMAN, root, arr, top); 
}

void for2 () {
    int cont = 0;
    while(cont < 256) {
    simbolos[cont] = cont;
    cont++;
    }

}

int main(int argc, char* argv[]){ 

    if (argc != 2){
        printf("Please insert just one frequency document.-------------------------");
        return 0;
    }

    char BufferCheck[44];

    FILE *Freqfile = fopen(argv[1], "rb");
    if (Freqfile == NULL){
        perror("Error opening frequency file----------------------------------------");
        return 0;
    }
    fgets(BufferCheck, sizeof(BufferCheck), Freqfile);
    if (strncmp( BufferCheck, "    Hex value   |   Symbol   |  Frequency \n", 44) != 0) {
        printf("Please insert a frequency file with the correct format.--------------");
        return 0;
        }

    extraerfreq(Freqfile);
    fclose(Freqfile);

    Sumfreq();



    if(FreqSum == 0){
        printf("This frequency file hasn't been loaded with any frequencies, please");
        printf("check the file and try again. --------------------------------------");
    }

    int size = 256; 



    
    FILE *HUFFMAN_CODES = fopen("HUFFMAN_CODES.h", "wb");
    if(HUFFMAN_CODES == NULL){
        perror("Error creating Huffman codes file----------------------------------------");
    } 

    FILE *HUFFMAN_TREE = fopen("HUFFMAN_TREE.h", "wb");
    if(HUFFMAN_TREE == NULL){
        perror("Error creating Huffman tree file----------------------------------------");
    } 

    for2();
    
    HuffmanCodes(size,HUFFMAN_CODES, HUFFMAN_TREE);

    fclose(HUFFMAN_CODES);
    fclose(HUFFMAN_TREE);

    return 0;

}



/*
1. freq
2. menores sumar
3. arbol
4. Escribir en .h que se llame igual al de frequencias

hex  | A = 1
hex  | B = 01
hex  | C = 011

DESPUES

Proyecto 4

ABC = {A} = 1 {B} = 01 101 {C} = 011 101011

Proyecto 5

101011 = {1} = A {01} = B {011} = C = ABC

*/


/*

    NODOSUMA(5)   
   / 0        \ 1  
NODITO(2)   NODITO(3)

*/