/*
Luiz Guilherme Enger
Engenharia Física - UFRGS  -  00218975
Engenharia da Computação - PUCRS  - 121057780

Gustavo Zanotto
Engenharia da Computação - PUCRS -
*/




// 08/11: criar uma lista encadeada ou array global que lista os 10 PCBs
// quando thread começar, pedir pro SO algum dos PCBs dessa lista

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
#define N_process 20
#define N_pages 10

// escolha do algoritmo de vitimacao:
// 1 para FIFO
// 2 para LRU
// 3 para Second chance
#define algoritmo 2

int total_page_fault = 0;
int total_hit = 0;

int index_FIFO = 0; // variavel global para utilizar algoritmo de vitimacao FIFO
// vai começar em 0, ir a 99 e voltar a zero, pois a memoria vai funcionar como uma lista mesmo

int index_second_chance = 0; // vai guardar o indice de qual frame vai verificar primeiro

typedef struct frame // frame sabe qual pagina esta contida, de qual processo é a página
{                    // se esta sendo utilizado e controle para algoritmo second chance
    int page;
    int process_id;
    int used;
    int second_chance; // uso apenas para algoritmo de vitimação second-chance
}frame;

frame memory[N_frames]; // array de frames
frame frameaux;


sem_t mutex;  // mutex para gerenciar acesso à lista de frames

typedef struct page
{
    int frame_index; // em qual frame a pagina esta localizada, vai de 0 a 99
    int valid_bit; // valid_bit em '0', pagina n esta em nenhum frame; em '1' pagina esta no frame indicado por frame_index
}page;


typedef struct process_control_block // estrutura PCB, uma para cada thread
{
    page page_table[N_pages];     // o indice do page table é a página a ser lida; o conteúdo é o frame no qual a pagina se encontra
}process_control_block;

process_control_block PCBs[N_process];

int processos_criados = 0;

void Vitimar(int pid, int page)
{
    if (algoritmo == 1) // FIFO
    {
        // frame apontado pela variavel index_FIFO tera sua página substituída pelo nova
        // como a memória é um array, e a escrita começa do índice 0 ao 99, a variável que indica frame
        // a ser substituído vai ser incrementada a cada vitimação, voltando a 0 depois de 99
        // pois a própria escrita da memória vai ocorrer de maneira sequencial

        // primeiro deve-se alterar a page table do processo que vai "perder o frame"
        // trocando o bit de validade para 0, indicando que a pagina n esta mais na memoria
        PCBs[memory[index_FIFO].process_id].page_table[memory[index_FIFO].page].valid_bit = 0;

        // atualiza frame com id no processo que esta carregando pagina pra memoria e com a pagina
        memory[index_FIFO].process_id = pid;
        memory[index_FIFO].page = page;

        // atualiza page table do processo que esta carregando a pagina pra memoria
        PCBs[pid].page_table[page].valid_bit = 1;
        PCBs[pid].page_table[page].frame_index = index_FIFO;

        // avança valor do index_FIFO -> frame recem escrito será o "último" a ser substituído
        if(index_FIFO == N_frames-1) {index_FIFO = 0;}
          else index_FIFO++;

        printf("Frame %d foi vitimado, preenchido com page %d do processo %d!\n", index_FIFO, page, pid);

    }

    else if (algoritmo == 2) // LRU
    {
        //LRU --> deixar fixo o indice que vai ser vitimado
        // por exemplo, vitima sempre o indice 99
        // mas quando ocorrer alguma leitura de frame, o frame lido deve ser jogado pro indice 0 -> shiftar todo array?

        // primeiro invalidar na page table a pagina q vai ser vitimada
        PCBs[memory[N_frames-1].process_id].page_table[memory[N_frames-1].page].valid_bit = 0;

        // deslocar to
        for(int i=N_frames;i>0;i--)
        {
          memory[i].page = memory[i-1].page;                     //desloca array de frames
          memory[i].process_id = memory[i-1].process_id;
          memory[i].used = memory[i-1].used;
          memory[i].second_chance = memory[i-1].second_chance;
          PCBs[memory[i].process_id].page_table[memory[i].page].frame_index = i; //atualiza o frame_index de cada pagina ja que estao sendo deslocadas
        }

        memory[0].process_id = pid; //carrega a pagina no primeiro frame
        memory[0].page = page;

        PCBs[pid].page_table[page].valid_bit = 1;
        PCBs[pid].page_table[page].frame_index = 0;

    }

    else if(algoritmo == 3) // Second Chance
    {
        //Second chance
        // variavel index_second_chance é global e inicializada em 0
        while(memory[index_second_chance].second_chance == 1) // vai parar no primeiro com second_chance == 0
        {
          memory[index_second_chance].second_chance = 0;
          if(index_second_chance == N_frames-1)
          {index_second_chance = 0;}
          else
          index_second_chance++;
        }

        PCBs[memory[index_second_chance].process_id].page_table[memory[index_second_chance].page].valid_bit = 0;

        memory[index_second_chance].process_id = pid;
        memory[index_second_chance].page = page;

        PCBs[pid].page_table[page].valid_bit = 1;
        PCBs[pid].page_table[page].frame_index = index_second_chance;

        printf("Vitimou frame %d carregou page %d do process %d!\n", index_second_chance, page, pid);

        if(index_second_chance == N_frames-1)
        {index_second_chance = 0;}
        else
        index_second_chance++; // incrementa para começar a verificar no próximo frame,
    }                          // e não no frame que acabou de escrever

}

int CarregaPagina(int pid, int page) // carrega pagina no array de frames
{
  int i = 0;

    for(i=0;i<N_frames;i++)
    {
        if(memory[i].used == 0)
        {
            memory[i].used = 1;
            memory[i].page = page;
            memory[i].process_id = pid;

            PCBs[pid].page_table[page].valid_bit = 1;
            PCBs[pid].page_table[page].frame_index = i;
            i = 200;
        }
    }

    if(i == N_frames) // testou todos frames, todo array memory
    {Vitimar(pid, page);}
}



int LerPagina(int process_id, int page) // verifica se a pagina ja esta no array
{
  if(PCBs[process_id].page_table[page].valid_bit == 1)  // significa que a pagina esta "carregada" em algum frame
    {
      total_hit++;
      memory[PCBs[process_id].page_table[page].frame_index].second_chance = 1; // dá nova chance ao frame
      printf("Page %d from process %d found at frame %d!\n", page, process_id, PCBs[process_id].page_table[page].frame_index);

      if(algoritmo == 2) // LRU -> "shiftar" array de frame
      {
          frameaux = memory[PCBs[process_id].page_table[page].frame_index]; //frame auxiliar salva as info do ultimo frame lido
          // se da forma acima nao funcionar, descomentar abaixo
          // frameaux.page = memory[PCBs[process_id].page_table[page].frame_index].page;
          // frameaux.process_id = memory[PCBs[process_id].page_table[page].frame_index].process_id;
          // frameaux.used = memory[PCBs[process_id].page_table[page].frame_index].used;
          // frameaux.second_chance = memory[PCBs[process_id].page_table[page].frame_index].second_chance;

        for(int i=PCBs[process_id].page_table[page].frame_index;i>0;i--)
          {
          memory[i].page = memory[i-1].page;                     //desloca array de frames
          memory[i].process_id = memory[i-1].process_id;
          memory[i].used = memory[i-1].used;
          memory[i].second_chance = memory[i-1].second_chance;
          PCBs[memory[i].process_id].page_table[memory[i].page].frame_index = i; //atualiza o frame_index de cada pagina ja que estao sendo deslocadas
          }

          memory[0] = frameaux; //carrega a pagina do frameaux no primeiro frame
          // se acima nao funcionar, descomentar abaixo
          // memory[0].page = frameaux.page;
          // memory[0].process_id = frameaux.process_id;
          // memory[0].used = frameaux.used;
          // memory[0].second_chance = frameaux.second_chance;

          PCBs[process_id].page_table[page].frame_index = 0;
      }
    } // "ler" a pagina == do nothing
    else
    {
      total_page_fault++;
      printf("Process %d page %d page fault!\n", process_id, page);
      CarregaPagina(process_id, page);
    }
}

int criaProcesso()
{
    processos_criados++;
    return (processos_criados-1);

}

void * rodaProcesso()
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

void initMem_PCBs()
{
  for(int i=0;i<N_frames;i++)
  {
    memory[i].used = 0; // iniciliza memoria livre
    memory[i].second_chance = 1;
  }

  for(int i=0;i<N_process;i++)
  {
    for(int j=0;j<N_pages;j++)
    {
      PCBs[i].page_table[j].valid_bit = 0;  // nenhuma pagina esta carregada na memoria
    }
  }
}

int main()
{
    srand( (unsigned)time(NULL) );

    initMem_PCBs();

    sem_init(&mutex,0,1); // inicializa semaforo com um "credito"

    pthread_attr_t attr;
    pthread_t tid[N_process];
    pthread_attr_init(&attr);

    for(int i=0;i<N_process;i++)
        {pthread_create(&(tid[i]), &attr, rodaProcesso, NULL);}

        for(long cont=0;cont<1000000000;cont++) {} // busy wait
      	for(int i=0;i<N_process;i++)
      		{
      			pthread_cancel(tid[i]);
      		}
    switch (algoritmo) {
      case 1: printf("\nAlgoritmo FIFO\n");
              break;
      case 2: printf("\nAlgoritmo LRU\n");
              break;
      case 3: printf("\nAlgoritmo Second Chance\n");
              break;
      default: printf("\nAlgoritmo invalido!\n");
              break;

    }
    printf("Total hit: %d\n", total_hit);
    printf("Total page fault: %d\n", total_page_fault);
}
