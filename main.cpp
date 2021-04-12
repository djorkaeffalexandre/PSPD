#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <cstring>
#include <algorithm>

// g++ -std=c++17 -O2 main.cpp -o prog

// Desativa todos os LOGS
#define DEBUG true
// Desativa a lista de variáveis
#define LISTS true

using namespace std;

void check(
  int n_variaveis,
  int n_clausulas,
  vector<vector<int> > clausulas,
  vector<int> valores
) {
  vector<int> idx_clausulas_nao_satisfeitas;
  vector<int> valores_negativos(n_variaveis, 0);
  int n_valores_negativos = 0;

  bool flag_expressao = true;
  for (int i = 0; i < clausulas.size(); i++) {
    bool flag_clausula = false;

    vector<int> valores_negativos_clausula;
    for (int j = 0; j < clausulas[i].size(); j++) {
      int base = clausulas[i][j];
      
      int value = valores[abs(base) - 1];
      if (base < 0) {
        value *= -1;
      }

      if (value > 0) {
        // Se encontramos um valor positivo a cláusula é verdadeira
        // e portanto os valores negativos não importam mais
        // então podemos ir para a próxima cláusula
        flag_clausula = true;
        break;
      } else {
        #if LISTS
        valores_negativos_clausula.push_back(abs(base));
        #endif
      }
    }

    if (flag_clausula) {
      // Do nothing
    } else {
      flag_expressao = false;
      idx_clausulas_nao_satisfeitas.push_back(i);
      #if LISTS
      for (int k = 0; k < valores_negativos_clausula.size(); k++) {
        int base = valores_negativos_clausula[k];
        if (valores_negativos[base - 1] == 0) {
          n_valores_negativos += 1;
        }
        valores_negativos[base - 1] += 1;
      }
      #endif
    }
  }

  if (flag_expressao) {
    #if DEBUG
    printf("SAT\n");
    #endif
  } else {
    int n_clausulas_nao_satisfeitas = idx_clausulas_nao_satisfeitas.size();
    #if DEBUG
    printf("[%d clausulas falsas] ", n_clausulas_nao_satisfeitas);
    #endif
    for (int i = 0; i < n_clausulas_nao_satisfeitas; i++) {
      #if DEBUG
      printf("%d", idx_clausulas_nao_satisfeitas[i]);
      #endif
      if (i != n_clausulas_nao_satisfeitas - 1) {
        #if DEBUG
        printf(" ");
        #endif
      }
    }
    #if DEBUG
    printf("\n");
    #endif

    #if LISTS
    #if DEBUG
    printf("[lits] ");
    #endif
    while (n_valores_negativos > 0) {
      int max = distance(
        valores_negativos.begin(),
        max_element(
          valores_negativos.begin(),
          valores_negativos.end(),
          [](const int& a, const int& b) {
            return abs(a) <= abs(b);
          }
        )
      );
      printf("%d", valores[max] * -1);
      valores_negativos[max] = 0;
      n_valores_negativos--;
      if (n_valores_negativos > 0) {
        #if DEBUG
        printf(" ");
        #endif
      }
    }
    #if DEBUG
    printf("\n");
    #endif
    #endif
  }
}

int main() {
  int n_variaveis, n_clausulas;
  
  scanf("%d", &n_variaveis);
  scanf("%d", &n_clausulas);

  vector<vector<int> > clausulas;
  vector<int> variaveis;

  for (int i = 0; i < n_clausulas; i++) {
    int num;
    do {
      scanf("%d", &num);
      if (num != 0) {
        variaveis.push_back(num);
      }
    } while (num != 0);
    clausulas.push_back(variaveis);
    variaveis.clear();
  }

  char comando[4];
  vector<int> valores(n_variaveis);
  while(scanf("%4s", comando) != EOF) {
    if (strcmp(comando, "full") == 0) {
      for (int i = 0; i < n_variaveis; i++) {
        int variavel;
        scanf("%d", &variavel);
        valores[i] = variavel;
      }
      check(n_variaveis, n_clausulas, clausulas, valores);
    }
    if (strcmp(comando, "flip") == 0) {
      int variavel;
      scanf("%d", &variavel);
      valores[abs(variavel) - 1] = valores[abs(variavel) - 1] * -1;
      check(n_variaveis, n_clausulas, clausulas, valores);
    }
  }

  return 0;
}
