#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    char letter;
    int value;
    struct node *left;
    struct node *right;
    char *code;
} Node;

typedef struct heap {
  int capacity;
  int size;
  Node **array;
} Heap;

typedef struct array2d {
    int size;
    int **table;
} array2d;

typedef struct codeTable {
  char *huffCode;
  char letter;
} codeTable;

typedef struct stringarray {
  int size;
  codeTable **array;
} stringArray;

typedef struct closeHashTable {
  int size;
  Node **hashTable;
} closeHashTable;

int hash(char letter) {
  return (letter % 128);
}

stringArray *initializeStringArray(int capacity) {
  stringArray *stringarray = malloc(sizeof(stringArray));
  stringarray->size = capacity;
  stringarray->array = malloc(sizeof(codeTable *)*capacity);

  return stringarray;
}

void deletestringArray(stringArray *stringarr) {
  for (int i=0; i<stringarr->size;i++) {
    free(stringarr->array[i]);
  }
  free(stringarr->array);
  free(stringarr);
}


codeTable *initializecodeTable(char *huffCode, char letter) {
  codeTable *table = malloc(sizeof(codeTable));
  table->huffCode = malloc(sizeof(char)*(strlen(huffCode)+1));
  strcpy(table->huffCode, huffCode);
  table->letter = letter;

  return table;
}

array2d *initializearray2d(int capacity) {
  array2d *array2d = malloc(sizeof(array2d));
  array2d->size = capacity;
  array2d->table = malloc(sizeof(int *)*capacity);
  for (int i=0;i<array2d->size;i++) {
    array2d->table[i] = malloc(sizeof(int)*2);
  }
  return array2d;
}

Node *initializeNode(char letter, int value) {
  Node *newNode = malloc(sizeof(Node));
  newNode->left = NULL;
  newNode->right = NULL;
  newNode->code = NULL;
  newNode->value = value;
  newNode->letter = letter;
  return newNode;
}

void increaseCapacity(array2d *letter) {
  letter->size++;
  letter->table = realloc(letter->table, sizeof(int *)*letter->size);
  letter->table[letter->size-1] = malloc(sizeof(int)*2);
}

int parent(Heap *myHeap, int index) {
  int parentIndex = (index-1)/2;
  return parentIndex;
}

Heap *initialize(int capacity) {
  Heap *myHeap = malloc(sizeof(Heap));
  myHeap->capacity = capacity;
  myHeap->size = 0;
  myHeap->array = malloc(sizeof(Node *)*capacity);
  return myHeap;
}

void swap(Node **array, int index1, int index2) {
  Node *temp = array[index1];
  array[index1] = array[index2];
  array[index2] = temp;
}

void upheap(Heap *myHeap, int index) {
  if (index == 0) {
    return;
  }
  int parentIndex = parent(myHeap, index);
  Node *parentValue = myHeap->array[parentIndex];
  if (parentValue->value <= myHeap->array[index]->value) {
    return;
  }
  swap(myHeap->array, index, parentIndex);
  upheap(myHeap, parentIndex);
}

void insert(Heap *myHeap, Node *value) {
  myHeap->array[myHeap->size] = value;
  upheap(myHeap,myHeap->size);
  myHeap->size++;
}

int minChildIndex(Heap *myHeap, int index) {
  if (myHeap->size-1 >= (index+1)*2) {
    if(myHeap->array[(index+1)*2]->value >= myHeap->array[(index+1)*2-1]->value) {
      return (index+1)*2-1;
    }
    else {
      return (index+1)*2;
    }
  }
  else if (myHeap->size-1 == (index+1)*2-1) {
    return (index+1)*2-1;
  }
  else {
    return -1;
  }
}

void downheap(Heap *myHeap, int index) {
  if (myHeap->size-1 < (index+1)*2-1) {
    return;
  }
  int minChild = minChildIndex(myHeap,index);
  if (myHeap->array[index]->value <= myHeap->array[minChild]->value) {
    return;
  }
  swap(myHeap->array,index,minChild);
  downheap(myHeap,minChild);
}

void deletemin(Heap *myHeap) {
  swap(myHeap->array,0,myHeap->size-1);
  myHeap->size--;
  downheap(myHeap,0);
}

void printPreorderHelper(Node *node) {
  if (node != NULL) {
    printf("%d\n", node->value);
    printPreorderHelper(node->left);
    printPreorderHelper(node->right);
  }
  if (node ==NULL) {
    printf("hello\n");
  }
}

void printPreorder(Node *root) {
  printPreorderHelper(root);
}

void deleteHeap(Heap *myHeap) {
  for (int i; i<myHeap->size;i++) {
    free(myHeap->array[i]);
  }
  free(myHeap);
}

void delete2dArray(array2d *array) {
  for (int i=0;i<array->size;i++) {
    free(array->table[i]);
  }
  free(array);
}

void *hCode(Node *ptr, char *code, closeHashTable *codeTable) {
  
  if (ptr->left == NULL && ptr->right == NULL) {

    int key = hash(ptr->letter);

    Node *newNode = initializeNode(ptr->letter, 0);
    newNode->code = malloc(sizeof(char)*(strlen(code)+1));
    strcpy(newNode->code, code);
    codeTable->hashTable[key] = newNode;
  }

  int len=strlen(code);
  char *codeNew = malloc(sizeof(char)*(len+2));
  strcpy(codeNew,code);
  if (ptr->left != NULL) {
    codeNew[len]='0';
    codeNew[len+1]='\0';
    hCode(ptr->left, codeNew, codeTable);
  }
  if (ptr->right != NULL) {
    codeNew[len]='1';
    codeNew[len+1]='\0';
    hCode(ptr->right, codeNew, codeTable);
  }
  free(codeNew);
}

int main(int argc, char **argv)
{    
    /*----------------ENCODER-----------------------*/

    if (strcmp(argv[1], "encode") == 0) {
      char inputTextFilePath[strlen(argv[2]+1)];
      char codeTableFilePath[strlen(argv[3]+1)];
      char encodedFilePath[strlen(argv[4]+1)];
      strcpy(inputTextFilePath, argv[2]);
      strcpy(codeTableFilePath, argv[3]);
      strcpy(encodedFilePath, argv[4]);

      FILE *inputFile = fopen(inputTextFilePath, "r");
      if (inputFile == NULL)
      {
        printf("Could not open file to read: %s\n",inputTextFilePath);
        return 0;
      }

      char c;
      array2d *letter;
      letter = initializearray2d(1);
      int numOfCharacters = 0;
      while ((c = fgetc(inputFile)) != EOF && c!='\n')
      {
        int found = 0;

        for (int i=0; i<letter->size;i++) {
          if (letter->table[i][0] == c) {
            letter->table[i][1]++;
            found = 1;
          }
        }

        if (found == 0) {
          letter->table[letter->size-1][0] = c;
          letter->table[letter->size-1][1] = 1;
          numOfCharacters++;
          increaseCapacity(letter);
        }
      }
      fclose(inputFile);
      Heap *myHeap = initialize(numOfCharacters);
      for (int i = 0; i<numOfCharacters;i++) {
        Node *newNode = initializeNode(letter->table[i][0], letter->table[i][1]);
        insert(myHeap, newNode);      
      }

      while (myHeap->size > 2) {
        Node *left = myHeap->array[0];
        deletemin(myHeap);
    
        Node *right = myHeap->array[0];
        deletemin(myHeap);

        Node *tree = malloc(sizeof(Node));
        tree->value = left->value + right->value;
        tree->left = left;
        tree->right = right;
        Node *root = tree;
        insert(myHeap,tree);
      }

      Node *root = malloc(sizeof(Node));
      Node *left = myHeap->array[0];
      deletemin(myHeap);  
      Node *right = myHeap->array[0];
      deletemin(myHeap);
      root->left = left;
      root->right = right;
      root->value = left->value + right->value;


      closeHashTable *codeTable = malloc(sizeof(closeHashTable));
      codeTable->hashTable = malloc(sizeof(Node *)*128);
      for (int i=0;i<128;i++) {
        codeTable->hashTable[i] = NULL;
      }
  
      hCode(root, "", codeTable);    

      FILE *codeTableFile = fopen(codeTableFilePath, "w");
      if (codeTableFile == NULL)
      {
          printf("Could not open file to write: %s\n",codeTableFilePath);
          return 0;
      }

      for(int i = numOfCharacters - 1; i >= 0; i--)
      {
        fprintf(codeTableFile, "%c,%s,%d\n", letter->table[i][0], codeTable->hashTable[letter->table[i][0]]->code, letter->table[i][1]);
      }
      fclose(codeTableFile);

      inputFile = fopen(inputTextFilePath, "r");

      char *codeNew = malloc(sizeof(char));
      strcpy(codeNew, "");
      int count = 0;
      while ((c = fgetc(inputFile)) != EOF && c!='\n') {

        codeNew = realloc(codeNew, sizeof(char)*(strlen(codeNew)+strlen(codeTable->hashTable[c]->code)+1));
        strcat(codeNew,codeTable->hashTable[c]->code);    
        count++;
      }

      fclose(inputFile);
    
      FILE *encodedFile = fopen(encodedFilePath, "w");
      if (encodedFile == NULL)
      {
          printf("Could not open file to write: %s\n",encodedFilePath);
          return 0;
      }
      
      fprintf(encodedFile, "%s\n", codeNew);
      
      fclose(encodedFile);

      printf("Original: %d bits\n", count*8); 
      printf("Compressed: %ld bits\n", strlen(codeNew)); 
      printf("Compression Ratio: %.2f%%\n", (float)strlen(codeNew)/((float)count*8)*100); 
    

      delete2dArray(letter);
      deleteHeap(myHeap);      

    }
    /*----------------DECODER-----------------------*/
    else {
      char codeTableFilePath[strlen(argv[2]+1)];
      char encodedFilePath[strlen(argv[3]+1)];
      char decodedFilePath[strlen(argv[4]+1)];
      strcpy(codeTableFilePath, argv[2]);
      strcpy(encodedFilePath, argv[3]);
      strcpy(decodedFilePath, argv[4]);


      FILE *encodedFile = fopen(encodedFilePath, "r");
      if (encodedFile == NULL)
      {
        printf("Could not open file to read: %s\n",encodedFilePath);
        return 0;
      }

      char c;
      char *encodedCode = malloc(sizeof(char));
      strcpy(encodedCode, "");
      int i=1;
      while ((c = fgetc(encodedFile)) != EOF && c!='\n') {
        i++;
        encodedCode = realloc(encodedCode, sizeof(char)*i); 
        strcat(encodedCode,&c);     
        
      }
      fclose(encodedFile);

      FILE *codeTableFile = fopen(codeTableFilePath, "r");
      if (codeTableFile == NULL)
      {
        printf("Could not open file to read: %s\n",codeTableFilePath);
        return 0;
      }

      char str[256];
      char *characterCode = malloc(sizeof(char));

      stringArray *stringarr = initializeStringArray(1);      
      i = 1;
      
      while(fgets(str, 256, codeTableFile) != NULL) {

        int n=0;
     
        stringarr->array = realloc(stringarr->array, sizeof(codeTable *)*i);
        stringarr->size++;
        char *token = strtok(str, ",");
        char *huffCode;
        while (token != NULL) {
          if (n==0) {
            strcpy(characterCode, token);     
          }
          if (n==1) {
            huffCode = malloc(sizeof(char)*(strlen(token)+1));
            strcpy(huffCode, token);
            
          }
                   
          token = strtok(NULL, ",");			
          n++;
        }
        codeTable *codetable = initializecodeTable(huffCode, *characterCode); 
        stringarr->array[i-1] = codetable;
        i++;
      }

      fclose(codeTableFile);

      stringarr->size--;
      
      int start = 0;
      int stop = 0;
      int count=0;

      char *encodedCodeNew = malloc(sizeof(char)*(strlen(encodedCode)+1));

      FILE *decodedFile = fopen(decodedFilePath, "w");
      
      while (stop < strlen(encodedCode)) {

        count = 0;
                
        for (int i=start;i<=stop;i++) {
          encodedCodeNew[i-start] = encodedCode[i];
          count++;
          
        }
        encodedCodeNew[count] = '\0';
      
        for (int i = 0; i<stringarr->size;i++) {
                
          if (strcmp(encodedCodeNew,stringarr->array[i]->huffCode) == 0) {
            fprintf(decodedFile, "%c", stringarr->array[i]->letter);
            start = stop+1;
          }
        }
      stop++;      
      } 

      fclose(decodedFile);
      deletestringArray(stringarr);
    }
}
