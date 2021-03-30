#ifndef COMMON_H_
#define COMMON_H_

typedef struct {
  int inicio;
  int fim;
} Range;

int abs(int num);
int maior_num_vetor(int* array, int tamanho);
Range calcula_range(int atual, int tamanho, int pedaco);

#endif /* COMMON_H_ */
