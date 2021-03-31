#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <verifier.h>

#define SHARED 1
#define N_CONSUMERS 3
#define N_PRODUCERS 1

#define BUFFER_LEN 3
#define ENTRIES_LEN 10

void *Producer();
void *Consumer();

sem_t *empty, *full, *prod, *cons;

int** clausulas;

int n_clausulas, n_variaveis;
int* buffer[BUFFER_LEN];
Response responses[ENTRIES_LEN];
int i=0, f=0, idx=0, isEnd=0;

int main()
{
  pthread_t consumers[N_CONSUMERS], producers[N_PRODUCERS];

  empty = sem_open("/empty", O_CREAT, 0644, BUFFER_LEN);
  full = sem_open("/full", O_CREAT, 0644, 0);
  prod = sem_open("/prod", O_CREAT, 0644, 1);
  cons = sem_open("/cons", O_CREAT, 0644, 1);


  scanf("%d", &n_clausulas);
  scanf("%d", &n_variaveis);

  clausulas = (int **) calloc(n_clausulas, sizeof(int*));

  for(int i = 0; i < n_clausulas; i++) {
    clausulas[i] = (int *) calloc(n_variaveis, sizeof(int));

    int num;
    do {
      scanf("%d", &num);
      if (num != 0) {
        clausulas[i][abs(num) - 1] = num;
      }
    } while(num != 0);
  }

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
  for (int i = 0; i < idx; i++){
    printer(responses[i]);
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
    else if (strcmp(comando, "flip") == 0)
    {
      int variavel;
      scanf("%d", &variavel);
      valores[abs(variavel) - 1] = valores[abs(variavel) - 1] * -1;
    } else {
      continue;
    }
    sem_wait(empty);
    sem_wait(prod);
    f = (f + 1) % BUFFER_LEN;
    buffer[f] = (int *) calloc(n_variaveis, sizeof(int));
    memcpy(buffer[f], valores, sizeof(int) * n_variaveis);
    // printf("produzindo... %d\n", f);
    sem_post(prod);
    sem_post(full);
  }
  isEnd = 1;

  return NULL;
}

void *Consumer(void *no)
{
  while (isEnd == 0)
  {
    sem_wait(full);
    sem_wait(cons);
    i = (i + 1) % BUFFER_LEN;
    int* valores = buffer[i];
    Data data;
    data.n_clausules = n_clausulas;
    data.n_variables = n_variaveis;
    data.clausulas = clausulas;
    data.valores = valores;
    Response response = verify(data);
    responses[idx] = response;
    idx++;
    sem_post(cons);
    sem_post(empty);
  }

  return NULL;
}
