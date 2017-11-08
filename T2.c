/*
Luiz Guilherme Enger
Engenharia Física - UFRGS  -  00218975
Engenharia da Computação - PUCRS  - 121057780

Gustavo Zanotto
Engenharia da Computação - PUCRS -
*/




// 08/11: criar uma lista encadeada ou array global que lista os 10 PCBs
// quando thread começar, pedir pro SO algum dos PCBs dessa lista


// implementação da memoria em lista duplamente encadeada, mas n circular!
// sem_wait(&mutex) e sem_post(&mutex) entre o processo de escrita

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

int total_page_fault = 0;
int total_hit = 0;

int index_ultimo_frame_escrito = 0; // variavel global para utilizar algoritmo de vitimacao FIFO

typedef struct frame
{
    int index;
    int process_id;
    int used = 0;
    int second_chance = 0; // uso apenas para algoritmo de vitimação second-chance
}frame;


struct frame memory[N_frames]; // array de frames


//int iterador = 0;  // para percorrer a lista de frames

sem_t mutex;  // mutex para gerenciar acesso à lista de frames


/*typedef struct node_mem
{
	struct node_mem * previous;
	struct node_mem * next;
    struct frame      frame;
}node_mem;

node_mem * head_mem; // sempre vai apontar pro primeiro frame da lista
node_mem * tail_mem; // sempre vai apontar pro ultimo frame da lista
*/
typedef struct page
{
    int frame_index; // em qual frame a pagina esta localizada, vai de 0 a 99
    int valid_bit = 0; // valid_bit em '0', pagina n esta em nenhum frame; em '1' pagina esta no frame indicado por frame_index
}page;

typedef struct process_control_block // estrutura PCB, uma para cada thread
{
    struct page page_table[N_pages];     // o indice do page table é a página a ser lida; o conteúdo é o frame no qual a pagina se encontra
}process_control_block;

struct process_control_block PCBs[N_process];

int processos_criados = 0;

void Vitimar(int pid, int page)
{
    if (algoritmo == 1)
    {
        //FIFO
    }

    else if (algoritmo == 2)
    {
        //LRU
    }

    else
    {
        //Second chance
    }

}

int CarregaPagina(int pid, int page) // retorna em qual frame a pagina foi escrita
{
  int i = 0;

    for(i=0;i<N_frames;i++)
    {
        if(memory[i].used == 0)
        {
            memory[i].used = 1;
            PCBs[pid].page_table[page].valid_bit = 1;
            PCBs[pid].page_table[page].frame_index = i;
            i = 200;
        }
    }

    if(i == N_frames) // testou todos frames, todo array memory
    {Vitimar(pid, page);}
}
    /*while(memory[i].used == 1 && i<=99)
    {
        // frame ja ocupado, avança pro próximo
        i++;
    }

    if(i > 99)
    {
        //vitimacao
    }

    else*/

}
   


int LerPagina(int process_id, int page) // retorna index do frame no qual pagina se encontra
{
  if(PCBs[process_id].page_table[page].valid_bit == 1)  // significa que a pagina esta "carregada" em algum frame
    {
      total_hit++;
      printf("Page %d from process %d found!\n", page, process_id);

    } // "ler" a pagina == do nothing
    else
    {
      total_page_fault++;
      CarregaPagina(process_id, page);
    }
}

int criaProcesso()
{
    processos_criados++;
    return (processos_criados-1);

}

void * rodaProcesso(void* cont)
{
    int pid = criaProcesso(); // para acessar o PCB especifico, basta usar PCBs[id]
    int num_pagina;
    while(1)
    {
      num_pagina = rand()%N_pages;  // pagina a ser lida eh aleatoria
     
      sem_wait(&mutex);
      LerPagina(pid, num_pagina);
      sem_post(&mutex);
    }
}


int main()
{
    srand( (unsigned)time(NULL) );
    sem_init(&mutex,0,1); // inicializa semaforo com um "credito"
    for(int i=0;i<N_frames;i++)   // inicializa a "memoria", carregando os indices de cada frame
    {
      memory[i].index = i;
    }
    pthread_attr_t attr;
    pthread_t tid[N_process];
    pthread_attr_init(&attr);

    while(cont < N_process) // loop para criar todas as threads
    {
        pthread_create(&(tid[cont]), &attr, rodaProcesso, cont);
        cont++;
    }
}
