#include "common.h"

int abs(int num) {
  if (num < 0) {
    return num * -1;
  }
  return num;
}

int maior_num_vetor(int* array, int tamanho) {
  int idx_maior = 0;
  for(int i=0; i < tamanho; i++){
    if(abs(array[i]) >= abs(array[idx_maior])){
      idx_maior = i;
    }
  }
  return idx_maior;
}

Range calcula_range(int atual, int tamanho, int pedaco) {
  int begin = (tamanho * atual) / pedaco;
  int end = ((tamanho * (atual + 1)) / pedaco) - 1;
  Range range;
  range.inicio = begin;
  range.fim = end;
  return range;
}
