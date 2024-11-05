#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 100
#define DEFAULT_MAX_SIZE 1024

char* HuffmanCodesArray[256];

unsigned char Arbol[1000000] = {0};

int huffmanTreeChildren[10000][2];

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

typedef struct StackNode {
    MinHeapNode* treeNode;
    struct StackNode* next;
} StackNode;

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

void printHuffmanTree(MinHeapNode* root, int level) {
    if (root != NULL) {
        for (int i = 0; i < level; i++) printf("   ");  // Indentación para visualizar el nivel
        printf("(%c, %d)\n", root->data, root->frequency);
        printHuffmanTree(root->left, level + 1);
        printHuffmanTree(root->right, level + 1);
    }
}
// Funciones de manejo de archivos
void extraerfreq(FILE* Freq) { // Extrae las frecuencias guardadas en la tabla de frecuencias
    char linea[100];
    int valor_hex, frecuencia;

    rewind(Freq);

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

void storeCode(int arr[], int n, unsigned char symbol) {
    char* code = (char*)malloc(n + 1); 
    for (int i = 0; i < n; ++i) {
        code[i] = '0' + arr[i]; 
    }
    code[n] = '\0'; 
    HuffmanCodesArray[symbol] = code; 
}

void tablaHuffman(FILE* Huffman, MinHeapNode* root, int arr[], int top) {

    if (root->left) { 
        arr[top] = 0; 
        tablaHuffman(Huffman, root->left, arr, top + 1); 
    } 
    if (root->right) { 
        arr[top] = 1; 
        tablaHuffman(Huffman, root->right, arr, top + 1); 
    } 
    if (isLeaf(root)) { 
        storeCode(arr, top, root->data); 
        //printf("Símbolo: %c \n", root->data);
    } 
}


int esPrimera2 = 1;
int cont = 0;
void guardarArbol(FILE* file, MinHeapNode* root) {
    if (esPrimera2) { 
        
        fprintf(file, "#ifndef HUFFMAN_TREE_H\n#define HUFFMAN_TREE_H\n\n");
        fprintf(file, "typedef struct MinHeapNode {\n");
        fprintf(file, "    unsigned char data;\n");
        fprintf(file, "    struct MinHeapNode* left;\n");
        fprintf(file, "    struct MinHeapNode* right;\n");
        fprintf(file, "} MinHeapNode;\n\n");

        fprintf(file, "MinHeapNode* reconstruirArbol(const unsigned char* arr, int* index, int size);\n\n");
        fprintf(file, "const unsigned char huffmanTreeNodes[] = {\n");
        esPrimera2 = 0;
    }

    if (root == NULL) return;

    int currentIndex = cont;  

   
    if (isLeaf(root)) {
        fprintf(file, "    [ %d ] = 0x%02X,\n", cont, root->data);
    } else {
        fprintf(file, "    [ %d ] = '$',\n", cont);
    }

    
    huffmanTreeChildren[currentIndex][0] = root->left ? cont + 1 : -1;
    huffmanTreeChildren[currentIndex][1] = root->right ? cont + (root->left != NULL ? 2 : 1) : -1;

    cont++;

    
    guardarArbol(file, root->left);
    guardarArbol(file, root->right);
}

void escribirHijosEnArchivo(FILE* file) {
    fprintf(file, "const int huffmanTreeChildren[][2] = {\n");

  
    for (int i = 0; i < cont; i++) {
        fprintf(file, "    { %d, %d },\n", huffmanTreeChildren[i][0], huffmanTreeChildren[i][1]);
    }

    fprintf(file, "};\n\n");
    fprintf(file, "#endif // HUFFMAN_TREE_H\n");
}

/*
void guardarArbol(FILE* file, MinHeapNode* root) {
    if (esPrimera2) { 
        fprintf(file, "#ifndef HUFFMAN_TREE_H\n#define HUFFMAN_TREE_H\n\n");
        fprintf(file, "typedef struct MinHeapNode {\n");
        fprintf(file, "    unsigned char data;\n");
        fprintf(file, "    struct MinHeapNode* left;\n");
        fprintf(file, "    struct MinHeapNode* right;\n");
        fprintf(file, "} MinHeapNode;\n\n");

        fprintf(file, "MinHeapNode* reconstruirArbol(const unsigned char* arr, int* index, int size);\n\n");
        fprintf(file, "const unsigned char huffmanTreeNodes[] = {\n");
        esPrimera2 = 0;
    }

    if (root == NULL) return;
    
    if (isLeaf(root)) {
        //printf("Almacenando hoja en índice %d: símbolo = 0x%02X\n", cont, root->data);
        fprintf(file, "[%d] = 0x%02X, \n", cont, root->data);
    } else { 
        //printf("Almacenando nodo interno en índice %d: símbolo = '$'\n", cont);
        fprintf(file, "[%d] = '$', \n", cont);
    }

    cont++; 

    guardarArbol(file, root->left);
    guardarArbol(file, root->right);
}
*/

/*
int esPrimera3 = 1;
void guardarArbolFuncionePorfavor(MinHeapNode* root, int direccion, int nivel) {
    if (root == NULL) return;
    if (esPrimera3) {
        Arbol[0] = root->data;
        esPrimera3 = 0;
    }
    if (direccion == 0) {
        Arbol[(nivel * 2) + 1] = root->data;
        printf("Almacenando nodo interno en índice %d: símbolo = '0x%02X'\n", (nivel * 2) + 1, root->data);
    }
    else{
        Arbol[(nivel * 2) + 2] = root->data;
        printf("Almacenando nodo interno en índice %d: símbolo = '0x%02X'\n", (nivel * 2) + 2, root->data);
    }
    
    guardarArbolFuncionePorfavor(root->left, 1, nivel + 1);
    guardarArbolFuncionePorfavor(root->right, 0, nivel + 1);
    
}
*/

void guardarArbolFuncionePorfavor(MinHeapNode* root, int index) {
    if (root == NULL) return;

   
    Arbol[index] = root->data;
    printf("Almacenando nodo en índice %d: símbolo = '0x%02X'\n", index, root->data);

   
    guardarArbolFuncionePorfavor(root->left, 2 * index + 1);

    
    guardarArbolFuncionePorfavor(root->right, 2 * index + 2);
}

void printArbol(FILE* file) {
    fprintf(file, "#ifndef HUFFMAN_TREE_H\n#define HUFFMAN_TREE_H\n\n");
    fprintf(file, "const unsigned char huffmanTreeNodes[] = {\n");


    for (int i = 0; i < 1000000; i++) {
        if (Arbol[i] == '\0') {
            
        } else {
            if (Arbol[i] == '$') {
                fprintf(file, "'$', \n");
            } else {
                fprintf(file, "0x%02X, \n", Arbol[i]);
            }
        }
        
    }
}
int internalCount = 0;
void printTreePreOrder(MinHeapNode* root, int level) {
    if (root == NULL) return;
    internalCount++;
    printf("Nivel %d: símbolo = %c, frecuencia = %d\n", level, root->data, root->frequency);
    printTreePreOrder(root->left, level + 1);
    printTreePreOrder(root->right, level + 1);
}

void HuffmanCodes(int size, FILE *HUFFMAN, FILE *HuffmanTree) {  
    MinHeapNode* root = buildHuffmanTree(size); 

    guardarArbol(HuffmanTree, root);
    
    // El tamaño de este array está sujeto a la altura del árbol, por lo que hay que calcular más o menos cual sería un número al que nunca llegaría
    int arr[300], top = 0; 
    


    //guardarArbol(HuffmanTree, root);
    tablaHuffman(HUFFMAN, root, arr, top);

    fprintf(HUFFMAN, "#ifndef HUFFMAN_CODES_H\n#define HUFFMAN_CODES_H\n\n");
    fprintf(HUFFMAN, "const char* huffmanCodes[256] = {\n");
    for (int i = 0; i < 256; ++i) {
        if (HuffmanCodesArray[i] != NULL) {
            fprintf(HUFFMAN, "    [0x%02X] = \"%s\",\n", i, HuffmanCodesArray[i]);
        } else {
            fprintf(HUFFMAN, "    [0x%02X] = NULL,\n", i); // En caso de que no haya código
        }
    }
    fprintf(HUFFMAN, "};\n\n#endif\n");

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

    fprintf(HUFFMAN_TREE, "};\n");
    escribirHijosEnArchivo(HUFFMAN_TREE);

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