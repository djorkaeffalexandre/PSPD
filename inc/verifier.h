#ifndef VERIFIER_H_
#define VERIFIER_H_

#define TRUE 1
#define FALSE 0

typedef struct {
  int satisfies; // TRUE || FALSE
  int non_satisfied_clausules_count;
  int* non_satisfied_clausules;
  int non_satisfied_variables_count;
  int* non_satisfied_variables;
  int* non_satisfied_variables_base;
  int n_clausules;
  int n_variables;
  int* valores;
} Response;

typedef struct {
  int n_clausules;
  int n_variables;
  int** clausulas;
  int* valores;
} Data;

Response verify(Data data);
void printer(Response response);

#endif /* VERIFIER_H_ */
