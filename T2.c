/*
Luiz Guilherme Enger
Engenharia Física - UFRGS  -  00218975
Engenharia da Computação - PUCRS  - 121057780

Gustavo Zanotto
Engenhaira da Computação - PUCRS - 
*/

#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdlib.h>

#define N_frames 100
#define N_process 10
#define N_pages 10

typedef struct frame
{
    int index;
    int used = 0;
    int second_chance = 0; // uso apenas para algoritmo de vitimação second-chance
}frame;


typedef struct node_mem
{
	struct node_mem * previous;
	struct node_mem * next;
    struct frame      frame;
}node_mem;

node_mem * head_mem;
node_mem * ite_mem;
node_mem * tail_mem;

typedef struct page
{
    int frame_index; // em qual frame a pagina esta localizada, vai de 0 a 99
    int valid_bit = 0;
}page;

typedef struct process_control_block
{
    int pid;
    struct page page_table[N_pages];     // o indice do page table é a página a ser lida; o conteúdo é o frame no qual a pagina se encontra
}process_control_block;

void * rodaProcesso(void* cont)
{
    int id = (int)cont;
    process_control_block * process = malloc(sizeof(*process_control_block));
    process->pid = id;
    while(1)
    {


    

}

int main()
{
    pthread_attr_t attr;
    pthread_t tid[N_process];
    pthread_attr_init(&attr);

    int cont = 0;

    for(int i = 0; i<N_frames;i++)
    {
        node_mem * 
    
    while(cont < N_process)
    {
        pthread_create(&(tid[cont]), &attr, rodaProcesso, cont);
        cont++;
    }
        
