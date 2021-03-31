#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#define SHARED 1
#define N_CONSUMERS 3
#define N_PRODUCERS 1

#define BUFFER_LEN 3

void *Producer();
void *Consumer();

sem_t *empty, *full, *prod, *cons;

int n_clausulas, n_variaveis;
int* buffer[BUFFER_LEN];
int i=0, f=0;

int main()
{
  pthread_t consumers[N_CONSUMERS], producers[N_PRODUCERS];

  empty = sem_open("/empty", O_CREAT, 0644, BUFFER_LEN);
  full = sem_open("/full", O_CREAT, 0644, 0);
  prod = sem_open("/prod", O_CREAT, 0644, 1);
  cons = sem_open("/cons", O_CREAT, 0644, 1);


  scanf("%d", &n_clausulas);
  scanf("%d", &n_variaveis);

  for (int i = 0; i < N_PRODUCERS; i++) {
    pthread_create(&producers[i], NULL, Producer, NULL);
  }

  for (int i = 0; i < N_CONSUMERS; i++) {
    pthread_create(&consumers[i], NULL, Consumer, (void *)&i);
  }

  for (int i = 0; i < N_PRODUCERS; i++) {
    pthread_join(producers[i], NULL);
  }

  for (int i = 0; i < N_CONSUMERS; i++) {
    pthread_join(consumers[i], NULL);
  }

  printf("\nMain done\n");
  (void) sem_unlink("/empty");
  (void) sem_unlink("/full");
  (void) sem_unlink("/prod");
  (void) sem_unlink("/cons");
}

void *Producer()
{
  char comando[4];
  int* valores = (int *) calloc(n_variaveis, sizeof(int));
  while(scanf("%4s", comando) != EOF)
  {
    if (strcmp(comando, "full") == 0)
    {
      for (int i = 0; i < n_variaveis; i++)
      {
        int variavel;
        scanf("%d", &variavel);
        valores[i] = variavel;
      }
    }
    if (strcmp(comando, "flip") == 0)
    {
      int variavel;
      scanf("%d", &variavel);
      valores[abs(variavel) - 1] = valores[abs(variavel) - 1] * -1;
    }
    sem_wait(empty);
    sem_wait(prod);
    f = (f + 1) % BUFFER_LEN;
    buffer[f] = (int *) calloc(n_variaveis, sizeof(int));
    memcpy(buffer[f], valores, sizeof(int) * n_variaveis);
    // buffer[f] = valores;
    sem_post(prod);
    sem_post(full);
  }

  return NULL;
}

void *Consumer(void *no)
{
  while (1)
  {
    sem_wait(full);
    sem_wait(cons);
    i = (i + 1) % BUFFER_LEN;
    int* valores = buffer[i];
    sem_post(cons);
    sem_post(empty);
    printf("buffer %d: %d %d\n",idx, valores[0], valores[1]);
  }

  return NULL;
}
