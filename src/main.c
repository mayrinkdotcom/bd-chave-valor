#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define M 19

pthread_mutex_t lock;

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

void lockMutex(){
  pthread_mutex_lock(&lock);
}

void unlockMutex(){
  pthread_mutex_unlock(&lock);
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



void *insert() {
   Person p = readPerson();
   lockMutex();
   int index = hashCode(p.registration);
   while (hashTable[index].registration != -1)
      index = hashCode(index + 1);
   hashTable[index] = p;
   unlockMutex();
   return NULL;
}

Person *search(int key) {
   int index = hashCode(key);

   //lockMutex();
   while (hashTable[index].registration)
   {
      if (hashTable[index].registration == key) {
         //unlockMutex();
         return &hashTable[index];
      }
      else{
         index = hashCode(index + 1);
         //unlockMutex();
      }
         
   }
   

   return NULL;
}

void *print() {

   lockMutex();
   printf("\n-------------------------TABLE-------------------------\n");
   for (int i = 0; i < M; i++) {
      if (hashTable[i].registration != -1)
         printf("%2d = %3d \t %s", i, hashTable[i].registration, hashTable[i].name);
      else
         printf("%2d = \n", i);
   }
   printf("\n-------------------------------------------------------\n");
   unlockMutex();

   return NULL;  
}

void *writeFile(void * operation){
   FILE * arquivo;
   lockMutex();
   if ((arquivo = fopen("arquivo.txt","a")) == NULL){
     printf("Erro de abertura! \n");
   }else{
      fprintf(arquivo, "%s\n", operation);
      fclose(arquivo);
   }
   unlockMutex();
   return NULL;
}

void *readFile(){
   FILE * arquivo;
   char operation[50];
   lockMutex();
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
   unlockMutex();
   return NULL;
}




int main() {
   FILE * arquivo;
   printf("\nHello, World!\n");
   int op, key;
   Person *p;
   pthread_t writter;
   pthread_t req;

   initTable();

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
         pthread_create(&req, NULL, insert, NULL);
         pthread_create(&writter, NULL, writeFile, (void *)"Insert");

         pthread_join(req, NULL);
         pthread_join(writter, NULL);
         break;
      
      case 2:
         printf("Insert the key you want to search in the table: ");
         scanf("%d", &key);
         p = search(key);
         printf("%s",p->name);
         //p = pthread_create(&req, NULL, search, (void *) key);
         if (p){
            printf("\n\tRegistration: %d \tName: %s\n", p->registration, p->name);
            pthread_create(&writter, NULL, writeFile, (void *)"Search");
            pthread_join(writter, NULL);
            }
         else{
            printf("\nKey didn't match any record.\n");
            pthread_create(&writter, NULL, writeFile, (void *)"Search, no answer");
            pthread_join(writter, NULL);
            }
         break;
      
      case 3:
         print();
         pthread_create(&writter, NULL, writeFile, (void *)"Print");
         pthread_join(writter, NULL);
         break;         
      
      case 4:
         pthread_create(&req, NULL, readFile, NULL);
         pthread_join(req, NULL);
         break;
      case 0:
         printf("Bye...\n");
         break;
      
      default:
         printf("Invalid option!\n");
         pthread_create(&writter, NULL, writeFile, (void *) "Invalid option");
         break;
      }
    
   } while (op != 0);

   pthread_exit(NULL);
   return 0;
}