#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define M 19

typedef struct
{
   int registration;
   char name[50];
}Person;

Person hashTable[M];



void initTable() {
   for (int i = 0; i < M; i++)
      hashTable[i].registration = -1;
}

int hashCode(int key) {
   return key % M;
}

Person readPerson() {
   Person p;
   printf("Insert the registration: ");
   scanf("%d", &p.registration);
   scanf("%*c");
   printf("Insert the name: ");
   fgets(p.name, 50, stdin);
   return p;
}



void insert() {
   Person p = readPerson();
   int index = hashCode(p.registration);
   while (hashTable[index].registration != -1)
      index = hashCode(index + 1);
   hashTable[index] = p;
}

Person* search(int key) {
   int index = hashCode(key);
   while (hashTable[index].registration)
   {
      if (hashTable[index].registration == key)
         return &hashTable[index];
      else
         index = hashCode(index + 1);
   }
   return NULL;
}

void print() {
   printf("\n-------------------------TABLE-------------------------\n");
   for (int i = 0; i < M; i++) {
      if (hashTable[i].registration != -1)
         printf("%2d = %3d \t %s", i, hashTable[i].registration, hashTable[i].name);
      else
         printf("%2d = \n", i);
   }
   printf("\n-------------------------------------------------------\n");  
}
void writeFile(FILE * arquivo,char operation[50]){
   if ((arquivo = fopen("arquivo.txt","a")) == NULL){
     printf("Erro de abertura! \n");
   }else{
      fprintf(arquivo, "%s\n", operation);
      fclose(arquivo);
   }
}
void readFile(FILE * arquivo){
   char operation[50];
   if ((arquivo = fopen("arquivo.txt","r")) == NULL){
       printf("Erro de abertura! \n");
   }
   else{ 
      fgets(operation, 50, arquivo);
      while (!feof(arquivo)){ 
         printf("%s", operation);
         fgets(operation, 50, arquivo);
      }
   fclose(arquivo);
   }
}




int main() {
   FILE * arquivo;
   printf("\nHello, World!\n");
   int op, key;
   Person *p;
   pthread_t writter;

   initTable();

   pthread_create(&writter, NULL, writeFile  , NULL);
   do
   {
      printf("1 - Insert\n");
      printf("2 - Search\n");
      printf("3 - Print\n");
      printf("4 - See operations\n");
      printf("0 - Exit\n");
      printf("-> ");

      scanf("%d", &op);

      switch (op)
      {
      case 1:
         insert();
         writeFile(arquivo, "insert");
         break;
      
      case 2:
         printf("Insert the key you want to search in the table: ");
         scanf("%d", &key);
         p = search(key);
         if (p){
            printf("\n\tRegistration: %d \tName: %s\n", p->registration, p->name);
            writeFile(arquivo, "search");
            }
         else{
            printf("\nKey didn't match any record.\n");
            writeFile(arquivo, "search, no answer");
            }
         break;
      
      case 3:
         print();
         writeFile(arquivo, "print");
         break;         
      
      case 4:
         readFile(arquivo);
         break;
      case 0:
         printf("Bye...\n");
         break;
      
      default:
         printf("Invalid option!\n");
         break;
      }
    
   } while (op != 0);

   pthread_exit(NULL);
   return 0;
}