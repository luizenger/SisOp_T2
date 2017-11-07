/*
Luiz Guilherme Enger
Engenharia Física - UFRGS  -  00218975
Engenharia da Computação - PUCRS  - 121057780

Gustavo Zanotto
Engenhaira da Computação - PUCRS -
*/


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

typedef struct frame
{
    int index;
    int used = 0;
    int second_chance = 0; // uso apenas para algoritmo de vitimação second-chance
}frame;

//struct frame memory[N_frames];
//int iterador = 0;  // para percorrer a lista de frames
sem_t mutex;  // mutex para gerenciar acesso à lista de frames


typedef struct node_mem
{
	struct node_mem * previous;
	struct node_mem * next;
  struct frame      frame;
}node_mem;

node_mem * head_mem; // sempre vai apontar pro primeiro frame da lista
node_mem * tail_mem; // sempre vai apontar pro ultimo frame da lista

typedef struct page
{
    int frame_index; // em qual frame a pagina esta localizada, vai de 0 a 99
    int valid_bit = 0; // valid_bit em '0', pagina n esta em nenhum frame; em '1' pagina esta no frame indicado por frame_index
}page;

typedef struct process_control_block // estrutura PCB, uma para cada thread
{
    int pid; // identificador do processo, vai ir de 0 a N_process-1
    struct page page_table[N_pages];     // o indice do page table é a página a ser lida; o conteúdo é o frame no qual a pagina se encontra
}process_control_block;



int EscrevePagina(process_control_block * pcb, int num_pagina) // retorna em qual frame a pagina foi escrita
{
  sem_wait(&mutex);
  // como é lista encadeada, deve ser percorrida do inicio, primeiro frame vago é preenchido
  node_mem * iterador = head_mem;
  while((iterador->frame.used == 1)  && (iterador->frame.index <= 99)) // percorre toda lista, vai parar no primeiro com used = 0
  {
    iterador = iterador->next; // avança iterador para próximo!
  }
  // se chegou no fim significa que nao achou lugar disponivel
  if((iterador->frame.index == 99) && (iterador->frame.used == 1))
  {
    // algoritmo de vitimacao
    // ao remover determinada pagina de um frame ocupado, devemos pensar numa maneira de atualizar a page_table do processo que utilizava aquela frame
    // para indicar que sua pagina n esta mais carregada
  }
  else // significa que parou antes! entao pode escrever direto
  {
      iterador->frame.used = 1;  // "ocupa" o frame
      pcb->page_table[num_pagina].frame_index = iterador->frame.index;  // tabela recebe indice de qual frame a pagina esta ocupando
      pcb->page_table[num_pagina].valid_bit = 1;  // indica que pagina esta carregada em um frame
  }
  sem_post(&mutex);
}


int LerPagina(process_control_block * pcb, int num_pagina) // retorna index do frame no qual pagina se encontra
{
  if(pcb->page_table[num_pagina].valid_bit == 1)  // significa que a pagina esta "carregada" em algum frame
    {return pcb->page_table[num_pagina].frame_index} // "ler" a pagina == do nothing
    else
    {
      EscrevePagina(pcb, num_pagina);
    }
}

void * rodaProcesso(void* cont)
{
    int id = (int)cont;
    int num_pagina;
    process_control_block * process = malloc(sizeof(process_control_block));  // aloca nova estrutura PCB para cada thread
    process->pid = id;
    while(1)
    {
      num_pagina = rand()%10;  // pagina a ser lida/escrita eh aleatoria
      if ((rand()%10)>4)
        {EscrevePagina(process, num_pagina);}
      else
        {LerPagina(process, num_pagina);}
    }
}


void initMemory() // adiciona 98 nodos
{
  for(int i=1;i<N_frames-1;i++)
  {
    node_mem * new = malloc(sizeof(node_mem)); // aloca novo nodo e posiciona antes do ultimo
    new->next = tail_mem;
    new->previous = tail_mem->previous;
    tail_mem->previous->next = new;
    tail_mem->previous = new;
    new->frame.index = i;
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

    head_mem = malloc(sizeof(node_mem));
    tail_mem = malloc(sizeof(node_mem));

    head_mem->frame.index = 0;
    tail_mem->frame.index = 99;
    head_mem->next = tail_mem;
    tail_mem->previous = head_mem;
    initMemory();

    /*for(int i = 0; i<N_frames;i++)
    {
        node_mem *
    }*/
    while(cont < N_process) // loop para criar todas as threads
    {
        pthread_create(&(tid[cont]), &attr, rodaProcesso, cont);
        cont++;
    }
}
