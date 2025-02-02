#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory.h>
#include <time.h>
#include <vector>

#include "hubsolver.h"

using namespace std;
int main() {
  num_hubs = 2;
  ler_dados("instancias/inst5.txt");

  Sol s;

  calc_custo_dist();
  ordenar_nos();
  declara_hubs(s);

  melhor_hub(s);

  heu_cons_gul(s);
  calc_fo(s);

  imprimir_sol(s);

  return 0;
}

void ler_dados(const char *arq) {
  FILE *f = fopen(arq, "r");
  (void)fscanf(f, "%d", &num_nos);
  for (int i = 0; i < num_nos; i++) {
    vet_nos[i].id = i;
    (void)fscanf(f, "%lf %lf", &vet_nos[i].x, &vet_nos[i].y);
  }
  fclose(f);
}

void arqv_sol(Sol &s) {
  FILE *arq;
  arq = fopen("solucaoOtima.txt", "w");
  fprintf(arq, "n: %d      p: %d\n", num_nos, num_hubs);
  fprintf(arq, "FO:      %f\n", s.fo);
  fprintf(arq, "HUBS [%d", s.vet_hubs[0]);
  for (int i = 1; i < num_hubs; i++) {
    fprintf(arq, ",%d", s.vet_hubs[i]);
  }
  fprintf(arq, "]\n");
  printf("OR    H1    H2    DS    CUSTO\n");
  for (int i = 0; i < num_hubs; i++) {
    for (int j = 0; j < num_hubs; j++) {
      printf("%d      %d    %d      %d    %.2f\n", o, s.vet_hubs[i],
             s.vet_hubs[j], ds, mat_custo[i][j]);
    }
  }
}

void imprimir_sol(Sol &s) {
  printf("n: %d      p: %d\n", num_nos, num_hubs);
  printf("FO:      %.2f\n", s.fo);
  printf("HUBS [%d", s.vet_hubs[0]);
  for (int i = 1; i < num_hubs; i++) {
    printf(",%d", s.vet_hubs[i]);
  }
  printf("]\n");
  printf("OR  H1  H2  DS  CUSTO\n");
  for (int i = 0; i < num_nos; i++) {
    for (int j = 0; j < num_nos; j++) {
      int h1 = no_hub[i];
      int h2 = no_hub[j];
      printf("\n%d   %d   %d   %d   %.2f\n", i, h1, h2, j,
             mat_custo_total[i][j]);
    }
  }
}

void calc_custo_dist() {
  memset(&vet_med_custo, 0, sizeof(vet_med_custo));
  memset(&mat_custo, 0, sizeof(mat_custo));
  int dist;
  for (int i = 0; i < num_nos; i++) {
    for (int j = 0; j < num_nos; j++) {
      if (i != j)
        dist = DISTANCIA(vet_nos[i], vet_nos[j]);
      else
        dist = 0;
      vet_med_custo[i] += dist;
      mat_custo[i][j] = dist;
    }
  }
}

void ordenar_nos() {
  No aux;
  int aux2;

  for (int i = 0; i < num_nos; i++)
    vet_ind_no[i] = i;

  for (int i = 0; i < num_nos - 1; i++) {
    for (int j = 0; j < num_nos - i - 1; j++) {
      if (vet_med_custo[j] > vet_med_custo[j + 1]) {
        aux = vet_nos[j];
        vet_nos[j] = vet_nos[j + 1];
        vet_nos[j + 1] = aux;

        aux2 = vet_ind_no[j];
        vet_ind_no[j] = vet_ind_no[j + 1];
        vet_ind_no[j + 1] = aux2;

        float aux_custo = vet_med_custo[j];
        vet_med_custo[j] = vet_med_custo[j + 1];
        vet_med_custo[j + 1] = aux_custo;
      }
    }
  }
}

void declara_hubs(Sol &s) {
  for (int i = 0; i < num_hubs; i++)
    s.vet_hubs[i] = vet_ind_no[i];
}

void melhor_hub(Sol &s) {
  for (int i = 1; i < num_hubs; i++) {
    for (int j = 0; j < num_nos; j++) {
      if (DISTANCIA(vet_nos[j], vet_nos[s.vet_hubs[i]]) <
          DISTANCIA(vet_nos[j], vet_nos[s.vet_hubs[i - 1]]))
        no_hub[j] = s.vet_hubs[i];
      else
        no_hub[j] = s.vet_hubs[i - 1];
    }
  }
}

void heu_cons_gul(Sol &s) {
  for (int i = 0; i < num_nos; i++) {
    double dist_min = numeric_limits<double>::max();
    int melhor_h = -1;

    for (int j = 0; j < num_hubs; j++) {
      int hub = s.vet_hubs[j];
      if (mat_custo[i][hub] < dist_min) {
        dist_min = mat_custo[i][hub];
        melhor_h = hub;
      }
    }
    no_hub[i] = melhor_h;
  }
}

void calc_fo(Sol &s) {
  double custo_max = 0;

  for (int i = 0; i < num_nos; i++) {
    for (int j = 0; j < num_nos; j++) {
      int h1 = no_hub[i];
      int h2 = no_hub[j];
      int o_h1 = mat_custo[i][h1];
      int h1_h2 = ALPHA * mat_custo[h1][h2];
      int h2_ds = mat_custo[h2][j];

      double custo = o_h1 + h1_h2 + h2_ds;
      mat_custo_total[i][j] = custo;
      custo_max = max(custo_max, custo);
    }
  }
  s.fo = custo_max;
}

