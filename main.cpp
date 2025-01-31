#include <cmath>
#include <iostream>
#include <limits>
#include <memory.h>
#include <time.h>
#include <vector>

#define DISTANCIA(p1, p2)                                                      \
  (sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y)))

#include "hubGrasp.h"

using namespace std;

int main() {
  ler_dados("instancias/inst5.txt");

  Sol s;

  calc_custo_dist();
  ordenar_nos();
  heu_cons_gul(s);

  // teste leitura de dados
  for (int i = 0; i < num_nos; i++)
      printf("%d\n", vet_ind_no[i]);
    //  printf("%f %f\n", vet_nos[i].x, vet_nos[i].y);
    //  printf("%f\n",vet_med_custo[i]);
  imprimir_sol(s);
  
  return 0;
}
void imprimir_sol(Sol& s){
  printf("\nSolução: ");
  for (int i = 0; i < num_nos; i++)
    printf("%d ", s.vet_sol[i]);
  printf("\nHubs: ");
  for (int i = 0; i < num_nos; i++)
    printf("%d ", s.vet_hubs[i]);
  printf("\nFo: %d",s.fo);
}

void ler_dados(const char *arq) {
  FILE *f = fopen(arq, "r");
  (void)fscanf(f, "%d", &num_nos);
  for (int i = 0; i < num_nos; i++)
    (void)fscanf(f, "%lf %lf", &vet_nos[i].x, &vet_nos[i].y);
  fclose(f);
}

void calc_custo_dist() {
  memset(&vet_med_custo, 0, sizeof(vet_med_custo));
  for (int i = 0; i < num_nos; i++) {
    for (int j = 0; j < num_nos; j++) {
      vet_med_custo[i] += DISTANCIA(vet_nos[i], vet_nos[j]);
    }
  }
}

void ordenar_nos() {
  No aux;
  int aux2;

  for (int i = 0; i < num_nos; i++)
    vet_ind_no[i] = i;

  // teste de indice
  // for (int i = 0; i < num_nos; i++)
  //   printf("%d\n", vet_ind_no[i]);

  for (int i = 0; i < num_nos - 1; i++) {
    if (vet_med_custo[i] > vet_med_custo[i + 1]) {
      aux = vet_nos[i];
      vet_nos[i] = vet_nos[i + 1];
      vet_nos[i + 1] = aux;
      aux2 = vet_ind_no[i];
      vet_ind_no[i] = vet_ind_no[i + 1];
      vet_ind_no[i + 1] = aux2;
    }
  }
}

void heu_cons_gul(Sol &s) {
  memset(&s.vet_hubs, 0, sizeof(s.vet_hubs));
  memset(&s.vet_sol, -1, sizeof(s.vet_sol));
  for (int i = 0; i < num_hubs; i++) {
    s.vet_hubs[vet_ind_no[i]] = 1;
    for (int j = 0; j < num_nos; j++) {
      if (DISTANCIA(vet_nos[i], vet_nos[j]) <=
              DISTANCIA(vet_nos[i + 1], vet_nos[j]) &&
          i != j){
        s.vet_sol[j] = i+1;
        if(i>1)
          printf("%d,%d:%.2f %d,%d:%.2f\n",i,j,DISTANCIA(vet_nos[i], vet_nos[j]),i+1,j,DISTANCIA(vet_nos[i+1], vet_nos[j]));
      }
    }
  }
}
