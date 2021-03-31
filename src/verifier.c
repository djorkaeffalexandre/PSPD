#include <stdio.h>
#include <stdlib.h>
#include "verifier.h"
#include "common.h"

Response verify(Data data) {
  int n_clausulas = data.n_clausules;
  int n_variaveis = data.n_variables;
  int** clausulas = data.clausulas;
  int* valores = data.valores;

  int contador = 0;
  int clausula_satisfeita = 0;
  int contador_clausulas_nao_satisfeitas = 0;
  int contador_variaveis_nao_satisfeitas = 0;

  // Index de cláusulas não satisfeitas
  int* clausulas_nao_satisfeitas = (int*) calloc(n_clausulas, sizeof(int));
  int* variaveis_nao_satisfeitas = (int*) calloc(n_variaveis, sizeof(int));
  int* variaveis_nao_satisfeitas_base = (int*) calloc(n_variaveis, sizeof(int));

  for (int i = 0; i < n_clausulas; i++) {
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
        variaveis_nao_satisfeitas_base[j]++;
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
        variaveis_nao_satisfeitas[k] += variaveis_nao_satisfeitas_base[k];
        if(antes == 0 && variaveis_nao_satisfeitas[k] > 0){
          contador_variaveis_nao_satisfeitas++;
        }
      }
    }
  }

  Response response;
  response.valores = valores;
  response.n_clausules = n_clausulas;
  response.n_variables = n_variaveis;
  response.satisfies = contador == n_clausulas; // TRUE || FALSE
  response.non_satisfied_clausules_count = contador_clausulas_nao_satisfeitas;
  response.non_satisfied_clausules = clausulas_nao_satisfeitas;
  response.non_satisfied_variables_count = contador_variaveis_nao_satisfeitas;
  response.non_satisfied_variables = variaveis_nao_satisfeitas;
  response.non_satisfied_variables_base = variaveis_nao_satisfeitas_base;

  return response;
}

void printer(Response response) {
  int satisfies = response.satisfies;
  int contador_clausulas_nao_satisfeitas = response.non_satisfied_clausules_count;
  int contador_variaveis_nao_satisfeitas = response.non_satisfied_variables_count;
  int n_clausulas = response.n_clausules;
  int n_variaveis = response.n_variables;
  int* clausulas_nao_satisfeitas = response.non_satisfied_clausules;
  int* variaveis_nao_satisfeitas = response.non_satisfied_variables;
  int* valores = response.valores;

  // A cláusula foi satisfeita
  if (satisfies) {
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
