#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int abs(int num) {
  if (num < 0) {
    return num * -1;
  }
  return num;
}

int maior_num_vetor(int* array, int tamanho){
  int idx_maior = 0;
  for(int i=0; i < tamanho; i++){
    if(abs(array[i]) >= abs(array[idx_maior])){
      idx_maior = i;
    }
  }
  return idx_maior;
}

void verifica_satisfaz(
  int n_clausulas,
  int n_variaveis,
  int** clausulas,
  int* valores
) {
  int contador = 0;
  int clausula_satisfeita = 0;
  int contador_clausulas_nao_satisfeitas = 0;
  int contador_variaveis_nao_satisfeitas = 0;

  // Index de cláusulas não satisfeitas
  int* clausulas_nao_satisfeitas = (int*) calloc(n_clausulas, sizeof(int));
  int* variaveis_nao_satisfeitas = (int*) calloc(n_variaveis, sizeof(int));

  for (int i = 0; i < n_clausulas; i++) {
    int* variaveis_nao_satisfeitas_teste = (int*) calloc(n_variaveis, sizeof(int));
    for (int j = 0; j < n_variaveis; j++) {
      int variavel = clausulas[i][j];
      int valor;

      // Se a clausula não tiver essa variavel pula para a proxima
      if (variavel == 0){
        continue;
      }

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
        clausula_satisfeita = 1;
      } else {
        variaveis_nao_satisfeitas_teste[j]++;
      }
    }
    if (clausula_satisfeita == 1) {
      clausula_satisfeita = 0;
      contador++;
    } else {
      clausulas_nao_satisfeitas[i] = 1;
      contador_clausulas_nao_satisfeitas++;
      for(int k=0; k < n_variaveis; k++){
        int antes = variaveis_nao_satisfeitas[k];
        variaveis_nao_satisfeitas[k] += variaveis_nao_satisfeitas_teste[k];
        if(antes == 0 && variaveis_nao_satisfeitas[k] > 0){
          contador_variaveis_nao_satisfeitas++;
        }
      }
    }
  }
  // A cláusula foi satisfeita
  if (contador == n_clausulas) {
    printf("SAT\n");
  // A cláusula não foi satisfeita
  } else {
    // Imprime os índexes das cláusulas que não foram satisfeitas
    printf("[%d clausulas falsas] ", contador_clausulas_nao_satisfeitas);
    for (int i = 0; i < n_clausulas; i++) {
      if (clausulas_nao_satisfeitas[i] == 1) {
        printf("%d", i);
        contador_clausulas_nao_satisfeitas--;
        if (contador_clausulas_nao_satisfeitas > 0) {
          printf(" ");
        }
      }
    }
    printf("\n");
    // Print das variaveis
    printf("[lits] ");
    while(contador_variaveis_nao_satisfeitas > 0){
      int idx = maior_num_vetor(variaveis_nao_satisfeitas, n_variaveis);
      printf("%d", valores[idx] * -1);
      variaveis_nao_satisfeitas[idx] = 0;
      contador_variaveis_nao_satisfeitas--;
      if (contador_variaveis_nao_satisfeitas > 0) {
        printf(" ");
      }
    }
    printf("\n");
  }
}

int main() {
  int n_variaveis, n_clausulas;
  scanf("%d %d", &n_variaveis, &n_clausulas);

  // int[clausula][variaveis]
  int **clausulas = (int **) calloc(n_clausulas, sizeof(int*));

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

  char comando[4];
  int* valores = (int *) calloc(n_variaveis, sizeof(int));
  while(scanf("%4s", comando) != EOF) {
    if (strcmp(comando, "full") == 0) {
      for (int i = 0; i < n_variaveis; i++) {
        int variavel;
        scanf("%d", &variavel);
        valores[i] = variavel;
      }
      verifica_satisfaz(
        n_clausulas,
        n_variaveis,
        clausulas,
        valores
      );
    }
    if (strcmp(comando, "flip") == 0) {
      int variavel;
      scanf("%d", &variavel);
      valores[abs(variavel) - 1] = valores[abs(variavel) - 1] * -1;
      verifica_satisfaz(
        n_clausulas,
        n_variaveis,
        clausulas,
        valores
      );
    }
  }

  for (int i = 0; i < n_clausulas; i++) {
    free(clausulas[i]);
  }
  
  free(clausulas);
  free(valores);

  return 0;
}
