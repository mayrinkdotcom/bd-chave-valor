#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <conio.h>
#include <pthread.h>

// Bibilioteca para utilização de FIFO
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>

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

   do { // Impede que o usuário digite um número negativo
      printf("Insert the registration: ");
      scanf("%d", &p.registration);
      scanf("%*c");

      if(p.registration < 0) {
         printf("Invalid number! Insert a integer positive number!\n");
      }
   } while(p.registration < 0);

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
   for (int i = 0; i < M; i++) {
      if (hashTable[i].registration == key)
         return &hashTable[i];
   }

   static Person searchFailed;
   strcpy(searchFailed.name, "Not Found");
   searchFailed.registration = -1;

   return &searchFailed;
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


void *updateName(void *key){
  int index = hashCode((int)key);

   
    if (hashTable[index].registration == (int)key) {
      lockMutex();
      printf("Insert the name\n");
      scanf("%s", hashTable[index].name);
      printf("\n\tRegistration: %d \tName: %s\n", hashTable[index].registration, hashTable[index].name);  
      unlockMutex();  
    } else {
      printf("ERROR in updateName");
    }
   

  return NULL;
}


void *removeKey(void *key){
   int index = hashCode((int)key);

    if (hashTable[index].registration) {
         lockMutex();
         strcpy(hashTable[index].name, "");
         hashTable[index].registration = -1;
         unlockMutex();
    }
    else
         index = hashCode(index + 1);
    
   return NULL;
}


void *writeFile(void * operation){
   FILE * arquivo;
   lockMutex();
   if ((arquivo = fopen("arquivo.txt","a")) == NULL){
     printf("An error occurred opening the file! \n");
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
       printf("An error occurred opening the file! \n");
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

// Impede que o usuário digite um número negativo
int verifyEntry(char *action) {
   int key;
   do {
      printf("Insert the key you want to %s in the table: ", action);
      scanf("%d", &key);

      if(key < 0) {
         printf("Invalid number! Insert a integer positive number!\n");
      }
   } while(key < 0);

   return key;
}

void *cleanPrompt() {
   char answer;
   
   printf("Are you sure you want to clear the prompt? (y/n)\n");
   scanf(" %c", &answer);

   if(answer == 'y' || answer == 'Y') {
      system("cls");
   } else {
      printf("Opperation cancelled!\n");
   }

   return NULL;
}

int main() {
   FILE * arquivo;
   
   int op, key;
   Person *p;
   pthread_t writter;
   pthread_t req;

   pid_t pid;

   if(mkfifo("fifoFile", 0777)) {
      if(errno != EEXIST) {
         printf("An error occurred creating the file! \n");
         return 500;
      }
   }

   int fd = open("fifoFile", O_RDWR);
   if(fd == -1) {
      printf("An error occurred opening the file! \n");
      return 500;
   }
   
   initTable();

   do
   {
      printf("1 - Insert\n");
      printf("2 - Search\n");
      printf("3 - Print\n");
      printf("4 - Update Name\n");
      printf("5 - Remove\n");
      printf("6 - See operations\n");
      printf("7 - Clear prompt\n");
      printf("0 - Exit\n");
      printf("-> ");

      scanf("%d", &op);
      write(fd, &op, sizeof(op));
      
      switch (op)
      {
      case 1:
         pthread_create(&req, NULL, insert, NULL);
         pthread_create(&writter, NULL, writeFile, (void *)"Insert");

         pthread_join(req, NULL);
         pthread_join(writter, NULL);
         break;
      
      case 2:
         key = verifyEntry("search");

         p = search(key);
         // printf("%s",p->name);
         //p = pthread_create(&req, NULL, search, (void *) key);
         if (p && p->registration != -1){
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
         key = verifyEntry("update");

         p = search(key);
         if(p){
           printf("%s\n", p->name);
           pthread_create(&req, NULL, updateName, (void *) &key);
           pthread_create(&writter, NULL, writeFile, (void *)"Update");

           pthread_join(req, NULL);
           pthread_join(writter, NULL);
         }else{
            printf("\nKey didn't match any record.\n");
            pthread_create(&writter, NULL, writeFile, (void *)"Update, no answer");
            pthread_join(writter, NULL);
         }
         break;

      case 5:
         key = verifyEntry("remove");

         p = search(key);
         pthread_create(&req, NULL, removeKey, (void *) &p);
         pthread_create(&writter, NULL, writeFile, (void *)"Remove");

         pthread_join(req, NULL);
         pthread_join(writter, NULL);
         break;

      case 6:
         pthread_create(&req, NULL, readFile, NULL);
         pthread_join(req, NULL);
         break;

      case 7:
         pthread_create(&req, NULL, cleanPrompt, NULL);

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
   
   close(fd);
   pthread_exit(NULL);
   return 0;
}