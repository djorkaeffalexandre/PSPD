#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <pthread.h>

#define MAX_CMD_LEN 4
#define MAX_THREAD_LEN 4

using namespace std;

typedef struct {
  int inicio;
  int fim;
} Intervalo;

int n_variaveis;
int n_clausulas;
vector<vector<int> > clausulas;
vector<int> valores;
vector<int> clausulas_nao_satisfeitas;
vector<int> variaveis_nao_satisfeitas;
int n_variaveis_nao_satisfeitas = 0;
int n_satisfeitas = 0;

Intervalo calcula_intervalo(int atual, int tamanho, int pedaco) {
  int inicio = (tamanho * atual) / pedaco;
  int fim = ((tamanho * (atual + 1)) / pedaco) - 1;
  Intervalo intervalo;
  intervalo.inicio = inicio;
  intervalo.fim = fim;
  return intervalo;
}

bool verifica_clausula(vector<int> clausula, vector<int>* _variaveis_nao_satisfeitas) {
  bool satisfeita = false;

  for (int i = 0; i < n_variaveis; i++) {
    int variavel = clausula[i];

    // Se a clausula não tiver essa variavel pula para a proxima
    if (variavel == 0) {
      continue;
    }

    int valor;
    // Se a variável não for uma negação
    // Terá o valor igual ao valor lido
    if (variavel > 0) {
      valor = valores[abs(variavel) - 1];
    // Se a variável for uma negação
    // iremos dar um flip
    } else {
      valor = valores[abs(variavel) - 1] * -1;
    }

    // Se o valor é verdadeiro
    if (valor > 0) {
      satisfeita = true;
    } else {
      (*_variaveis_nao_satisfeitas)[i] += 1;
    }
  }
  return satisfeita;
}

void* verifica_expressao(void *aux) {
  int indice = *((int *) aux);
  Intervalo intervalo = calcula_intervalo(indice, n_clausulas, MAX_THREAD_LEN);

  for (int i = intervalo.inicio; i <= intervalo.fim; i++) {
    vector<int> _variaveis_nao_satisfeitas(n_variaveis, 0);
    bool satisfeita = verifica_clausula(clausulas[i], &_variaveis_nao_satisfeitas);
    if (satisfeita) {
      n_satisfeitas++;
    } else {
      clausulas_nao_satisfeitas.push_back(i);
      for (int k = 0; k < _variaveis_nao_satisfeitas.size(); k++) {
        int antes = variaveis_nao_satisfeitas[k];
        variaveis_nao_satisfeitas[k] += _variaveis_nao_satisfeitas[k];
        if (antes == 0 && variaveis_nao_satisfeitas[k] > 0) {
          n_variaveis_nao_satisfeitas++;
        }
      }
    }
  }

  return NULL;
}

int maior_num_vetor(vector<int> array, int tamanho) {
  int idx_maior = 0;
  for (int i = 0; i < tamanho; i++) {
    if (abs(array[i]) >= abs(array[idx_maior])) {
      idx_maior = i;
    }
  }
  return idx_maior;
}

void processa_comando() {
  // Reseta a contagem
  n_satisfeitas = 0;
  n_variaveis_nao_satisfeitas = 0;
  // Reseta buffer de não satisfeitas
  clausulas_nao_satisfeitas.clear();
  // Reseta buffer de variáveis
  memset(&variaveis_nao_satisfeitas[0], 0, variaveis_nao_satisfeitas.size() * sizeof variaveis_nao_satisfeitas[0]);

  pthread_t threads[MAX_THREAD_LEN];

  for (int i = 0; i < MAX_THREAD_LEN; i++) {
    int* arg = (int*) malloc(sizeof(int*));
    *arg = i;
    pthread_create(&threads[i], NULL, verifica_expressao, arg);
  }

  for (int i = 0; i < MAX_THREAD_LEN; i++) {
    pthread_join(threads[i], NULL);
  }

  // Se todas cláusulas foram satisfeitas
  int n_clausulas_nao_satisfeitas = clausulas_nao_satisfeitas.size();
  if (n_clausulas_nao_satisfeitas == 0) {
    printf("SAT\n");
  } else {
    // Imprime os índexes das cláusulas que não foram satisfeitas
    printf("[%d clausulas falsas] ", n_clausulas_nao_satisfeitas);
    sort(clausulas_nao_satisfeitas.begin(), clausulas_nao_satisfeitas.end());
    for (int i = 0; i < clausulas_nao_satisfeitas.size(); i++) {
      printf("%d", clausulas_nao_satisfeitas[i]);
      n_clausulas_nao_satisfeitas--;
      if (n_clausulas_nao_satisfeitas > 0) {
        printf(" ");
      }
    }
    printf("\n");
    // Imprime as variáveis que não possuem valor verdadeiro
    printf("[lits] ");
    while (n_variaveis_nao_satisfeitas > 0) {
      int idx = maior_num_vetor(variaveis_nao_satisfeitas, n_variaveis);
      printf("%d", valores[idx] * -1);
      variaveis_nao_satisfeitas[idx] = 0;
      n_variaveis_nao_satisfeitas--;
      if (n_variaveis_nao_satisfeitas > 0) {
        printf(" ");
      }
    }
    printf("\n");
  }
}

int main() {
  cin >> n_variaveis;
  cin >> n_clausulas;

  clausulas.resize(n_clausulas, vector<int>(n_variaveis));

  for(int i = 0; i < n_clausulas; i++) {
    int num;
    do {
      scanf("%d", &num);
      if (num != 0) {
        clausulas[i][abs(num) - 1] = num;
      }
    } while (num != 0);
  }

  char comando[MAX_CMD_LEN];
  valores.resize(n_variaveis);
  variaveis_nao_satisfeitas.resize(n_variaveis, 0);
  while(scanf("%4s", comando) != EOF) {
    if (strcmp(comando, "full") == 0) {
      for (int i = 0; i < n_variaveis; i++) {
        int variavel;
        scanf("%d", &variavel);
        valores[i] = variavel;
      }
    }
    if (strcmp(comando, "flip") == 0) {
      int variavel;
      scanf("%d", &variavel);
      valores[abs(variavel) - 1] = valores[abs(variavel) - 1] * -1;
    }
    processa_comando();
  }

  return 0;
}
