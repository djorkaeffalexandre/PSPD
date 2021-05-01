#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <cstring>
#include <algorithm>
#include <unordered_set>
#include <set>
#include <iterator>
#include <semaphore.h>
#include <pthread.h>

#define LITS true
#define PRINT true

#define THREADS 4

// g++ -std=c++17 -O2 -lpthread main.cpp -o prog

using namespace std;

typedef struct {
  int inicio;
  int fim;
} Intervalo;

int n_variaveis, n_clausulas;
vector<vector<int> > clausulas;
vector<vector<bool> > l_clausulas;
unordered_set<int> idx_clausulas_nao_satisfeitas;
#if LITS
vector<int> literais_falsos;
#endif
vector<bool> valores;
vector<Intervalo> intervalos;

// ==== THREADS ====
int valorFlip = -1;
bool rodando = true;
bool ehFlip = false;
sem_t semaforos_execucao[THREADS];
sem_t semaforos_impressao[THREADS];
pthread_t threads[THREADS];
// =================

void le_entrada();
void le_comandos();
void executa_full();
void executa_flip();
void imprime_resultado();
void calcula_intervalos();
void executa_comando(Intervalo intervalo);

// ==== THREADS ====
void inicia_threads();
void inicia_semaforos();
void finaliza_threads();
void combina_threads();
void destroi_semaforos();
void executa_threads();
// =================

bool compara(pair<int, int>& a, pair<int, int>& b) {
  if (a.second == b.second) {
    return abs(a.first) > abs(b.first);
  }
  return a.second > b.second;
}

Intervalo calcula_intervalo(int atual, int tamanho, int pedaco) {
  int inicio = (tamanho * atual) / pedaco;
  int fim = ((tamanho * (atual + 1)) / pedaco) - 1;
  Intervalo intervalo;
  intervalo.inicio = inicio;
  intervalo.fim = fim;
  return intervalo;
}

// ==== THREADS ====
void* thread(void* arg) {
  int indice = *((int *) arg);

  while (rodando) {
    sem_wait(&semaforos_execucao[indice]);
    if (rodando) {
      Intervalo intervalo = intervalos[indice];
      executa_comando(intervalo);
      sem_post(&semaforos_impressao[indice]);
    }
  }

  return NULL;
}
// =================

int main() {
  // ==== THREADS ====
  inicia_semaforos();
  inicia_threads();
  // =================

  le_entrada();
  calcula_intervalos();
  le_comandos();

  // ==== THREADS ====
  combina_threads();
  destroi_semaforos();
  // =================

  return 0;
}

void calcula_intervalos() {
  for (int i = 0; i < THREADS; i++) {
    Intervalo intervalo = calcula_intervalo(i, n_clausulas, THREADS);
    intervalos.push_back(intervalo);
  }
}

void le_entrada() {
  scanf("%d", &n_variaveis);
  scanf("%d", &n_clausulas);

  l_clausulas.resize(n_variaveis, vector<bool>(n_clausulas));
  valores.resize(n_variaveis, false);
  #if LITS
  literais_falsos.resize(n_variaveis, 0);
  #endif

  for (int i = 0; i < n_clausulas; i++) {
    vector<int> literais;
    int num;
    do {
      scanf("%d", &num);
      if (num != 0) {
        literais.push_back(num);
        l_clausulas[abs(num) - 1][i] = true;
      }
    } while (num != 0);
    clausulas.push_back(literais);
    literais.clear();
  }
}

void le_comandos() {
  char comando[4];
  while(scanf("%4s", comando) != EOF) {
    if (strcmp(comando, "full") == 0) {
      executa_full();
    }
    if (strcmp(comando, "flip") == 0) {
      executa_flip();
    }
    for (int i = 0; i < THREADS; i++) {
      sem_wait(&semaforos_impressao[i]);
    }
    imprime_resultado();
  }
  finaliza_threads();
}

void executa_full() {
  idx_clausulas_nao_satisfeitas.clear();
  #if LITS
  literais_falsos.clear();
  #endif
  int valor;
  for (int i = 0; i < n_variaveis; i++) {
    scanf("%d", &valor);
    valores[abs(valor) - 1] = valor > 0 ? true : false;
  }

  ehFlip = false;
  executa_threads();
}

void executa_flip() {
  int valor;
  scanf("%d", &valor);
  valores[abs(valor) - 1] = !valores[abs(valor) - 1];

  ehFlip = true;
  valorFlip = valor;
  executa_threads();
}

void executa_comando(Intervalo intervalo) {
  for (int i = intervalo.inicio; i < intervalo.fim; i++) {
    if (ehFlip) {
      if (!l_clausulas[abs(valorFlip) - 1][i]) {
        continue;
      }
    }
    bool satisfaz = false;
    for (int j = 0; j < clausulas[i].size(); j++) {
      int base = clausulas[i][j];

      int valor = valores[abs(base) - 1] ? base : base * -1;
      
      if (valor > 0) {
        satisfaz = true;
        break;
      }
    }
    if (satisfaz) {
      if (ehFlip) {
        if (idx_clausulas_nao_satisfeitas.find(i) != idx_clausulas_nao_satisfeitas.end()) {
          #if LITS
          for (int j = 0; j < clausulas[i].size(); j++) {
            int base = clausulas[i][j];
            bool flip = valores[abs(base) - 1];
            if (abs(base) == valorFlip) {
              flip = !flip;
            }
            int valor = flip ? base : base * -1;
            if (valor < 0) {
              literais_falsos[abs(base) - 1] -= 1;
            }
          }
          #endif
          idx_clausulas_nao_satisfeitas.erase(i);
        }
      }
    } else {
      bool deveExecutar = true;
      if (ehFlip) {
        deveExecutar = idx_clausulas_nao_satisfeitas.find(i) == idx_clausulas_nao_satisfeitas.end();
      }
      if (deveExecutar) {
        idx_clausulas_nao_satisfeitas.insert(i);
        #if LITS
        for (int j = 0; j < clausulas[i].size(); j++) {
          int base = clausulas[i][j];
          int valor = valores[abs(base) - 1] ? base : base * -1;
          if (valor < 0) {
            literais_falsos[abs(base) - 1] += 1;
          }
        }
        #endif
      }
    }
  }
}

void imprime_resultado() {
  if (idx_clausulas_nao_satisfeitas.empty()) {
    #if PRINT
    printf("SAT\n");
    #endif
  } else {
    int size = idx_clausulas_nao_satisfeitas.size();
    #if PRINT
    printf("[%d clausulas falsas]", size);
    #endif
    set<int> ordered(idx_clausulas_nao_satisfeitas.begin(), idx_clausulas_nao_satisfeitas.end());
    for (const auto& element: ordered) {
      #if PRINT
      printf(" %d", element);
      #endif
    }
    #if PRINT
    printf("\n");
    #endif
    #if LITS
    #if PRINT
    printf("[lits]");
    #endif
    vector<pair<int, int>> lits;
    for (int i = 0; i < n_variaveis; i++) {
      if (literais_falsos[i] > 0) {
        int literal = (i + 1) * (valores[i] ? 1 : -1) * -1;
        int contador = literais_falsos[i];
        lits.push_back(make_pair(literal, contador));
      }
    }
    sort(lits.begin(), lits.end(), compara);
    for (int i = 0; i < lits.size(); i++) {
      #if PRINT
      printf(" %d", lits[i].first);
      #endif
    }
    #if PRINT
    printf("\n");
    #endif
    #endif
  }
}

// MARK: - Threads

void inicia_semaforos() {
  for (int i = 0; i < THREADS; i++) {
    sem_init(&semaforos_execucao[i], 0, 0);
    sem_init(&semaforos_impressao[i], 0, 0);
  }
}

void destroi_semaforos() {
  for (int i = 0; i < THREADS; i++) {
    sem_destroy(&semaforos_execucao[i]);
    sem_destroy(&semaforos_impressao[i]);
  }
}

void inicia_threads() {
  for (int i = 0; i < THREADS; i++) {
    int* arg = (int*) malloc(sizeof(int*));
    *arg = i;
    pthread_create(&threads[i], NULL, thread, arg);
  }
}

void combina_threads() {
  for (int i = 0; i < THREADS; i++) {
    pthread_join(threads[i], NULL);
  }
}

void finaliza_threads() {
  rodando = false;
  for (int i = 0; i < THREADS; i++) {
    sem_post(&semaforos_execucao[i]);
  }
}

void executa_threads() {
  for (int i = 0; i < THREADS; i++) {
    sem_post(&semaforos_execucao[i]);
  }
}
