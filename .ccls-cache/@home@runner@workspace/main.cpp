#include "hubsolver.h"

using namespace std;
int main() {
  num_hubs = 50;
  const char *instancia = "instancias/inst200.txt";
  // const char* sol_oti="solucaoOtima.txt";

  ler_dados(instancia);

  // ler_sol(sol_oti, s);

  //Sol s;

  //calc_custo_dist();
  //ordenar_nos();

  //declara_hubs(s);
  //melhor_hub(s);

  //heu_cons_gul(s);

  //calc_fo(s);

  //imprimir_sol(s);
  //arqv_sol(s);

  // Sol s2;

  // clonar_sol(s,s2);
  // imprimir_sol(s2);
  
  teste_sol_i(instancia);
  //teste_sol_1000(instancia);

  return 0;
}

void ler_dados(const char *arq) {
  FILE *f = fopen(arq, "r");
  if (f == NULL) {
    printf("Erro ao abrir o arquivo de entrada.\n");
    return;
  }

  if (fscanf(f, "%d", &num_nos) != 1) {
    printf("Erro ao ler o número de nós.\n");
    fclose(f);
    return;
  }

  if (num_nos > MAX_NOS) {
    printf("Número de nós excede o limite.\n");
    fclose(f);
    return;
  }

  num_caminhos = num_nos * num_nos;

  for (int i = 0; i < num_nos; i++) {
    vet_nos[i].id = i;
    if (fscanf(f, "%lf %lf", &vet_nos[i].x, &vet_nos[i].y) != 2) {
      printf("Erro ao ler as coordenadas do nó %d.\n", i);
      fclose(f);
      return;
    }
  }

  fclose(f);
}

void arqv_sol(Sol &s) {
  FILE *arq;
  arq = fopen("solucaoOtima.txt", "w");

  fprintf(arq, "n: %d      p: %d\n", num_nos, num_hubs);
  fprintf(arq, "FO:      %.2f\n", s.fo);
  fprintf(arq, "HUBS [%d", s.vet_hubs[0]);
  for (int i = 1; i < num_hubs; i++) {
    fprintf(arq, ",%d", s.vet_hubs[i]);
  }
  fprintf(arq, "]\n");
  fprintf(arq, "OR  H1  H2  DS  CUSTO\n");
  for (int i = 0; i < num_nos * num_nos; i++) {
    fprintf(arq, "%d   %d   %d   %d   %.2f\n", s.cam[i].o, s.cam[i].h1,
            s.cam[i].h2, s.cam[i].ds, s.cam[i].custo);
  }

  fclose(arq);
}

void ler_sol(const char *nome_arquivo, Sol &s) {
  FILE *arq = fopen(nome_arquivo, "r");
  if (arq == NULL) {
    printf("o arquivo solucaoOtima.txt não existe.\n");
    return;
  }

  char buffer[100];

  if (fscanf(arq, "n: %d p: %d", &num_nos, &num_hubs) != 2) {
    printf("Erro ao ler n e p.\n");
    fclose(arq);
    return;
  }

  if (fscanf(arq, " FO: %lf", &s.fo) != 1) {
    printf("Erro ao ler a função objetivo (FO).\n");
    fclose(arq);
    return;
  }

  if (fscanf(arq, " HUBS [%d", &s.vet_hubs[0]) != 1) {
    printf("Erro ao ler hubs.\n");
    fclose(arq);
    return;
  }
  for (int i = 1; i < num_hubs; i++) {
    if (fscanf(arq, ",%d", &s.vet_hubs[i]) != 1) {
      printf("Erro ao ler hubs.\n");
      fclose(arq);
      return;
    }
  }

  if (fgets(buffer, sizeof(buffer), arq) == NULL) {
    printf("Erro ao ler fechamento dos hubs.\n");
    fclose(arq);
    return;
  }

  if (fgets(buffer, sizeof(buffer), arq) == NULL) {
    printf("Erro ao ler o cabeçalho dos caminhos.\n");
    fclose(arq);
    return;
  }

  int i = 0;
  while (i < num_nos * num_nos &&
         fscanf(arq, "%d %d %d %d %lf", &s.cam[i].o, &s.cam[i].h1, &s.cam[i].h2,
                &s.cam[i].ds, &s.cam[i].custo) == 5) {
    i++;
  }

  if (i == 0) {
    printf("Erro ao ler os caminhos. Nenhum caminho lido.\n");
  } else {
    printf("Leitura bem-sucedida! %d caminhos lidos.\n", i);
  }

  fclose(arq);
}

void clonar_sol(const Sol &s1, Sol &s2) {
  s2.fo = s1.fo;

  // Copiar os hubs
  for (int i = 0; i < num_hubs; i++) {
    s2.vet_hubs[i] = s1.vet_hubs[i];
  }

  // Copiar os caminhos
  for (int i = 0; i < num_nos * num_nos; i++) {
    s2.cam[i] = s1.cam[i];
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
  for (int i = 0; i < num_nos * num_nos; i++) {
    printf("%d   %d   %d   %d   %.2f\n", s.cam[i].o, s.cam[i].h1, s.cam[i].h2,
           s.cam[i].ds, s.cam[i].custo);
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

      if (i < MAX_NOS && j < MAX_NOS) {
        vet_med_custo[i] += dist;
        mat_custo[i][j] = dist;
      }
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

void heu_cons_gul(Sol &s) {
  for (int i = 0; i < num_nos; i++) {
    for (int k = 0; k < num_nos; k++) {
      int h1 = no_hub[i];
      int h2 = no_hub[k];
      int o_h1 = mat_custo[i][h1];
      int h1_h2 = ALPHA * mat_custo[h1][h2];
      int h2_ds = mat_custo[h2][k];

      double custo = o_h1 + h1_h2 + h2_ds;

      int cam = i * num_nos + k;
      s.cam[cam].custo = custo;
      s.cam[cam].o = i;
      s.cam[cam].h1 = h1;
      s.cam[cam].h2 = h2;
      s.cam[cam].ds = k;
    }
  }
}

void calc_fo(Sol &s) {
  double custo_max = 0;

  for (int i = 0; i < num_nos * num_nos; i++) {
    double custo = s.cam[i].custo;
    custo_max = max(custo_max, custo);
  }
  s.fo = custo_max;
}

void teste_sol_i(const char *instancia) {
  Sol s;

  ler_dados(instancia);

  clock_t inicio = clock();

  calc_custo_dist();
  ordenar_nos();

  declara_hubs(s);
  melhor_hub(s);

  heu_cons_gul(s);

  clock_t fim = clock();

  calc_fo(s);

  //imprimir_sol(s);
  //arqv_sol(s);

  double tempo_execucao = (double)(fim - inicio) / CLOCKS_PER_SEC;


  printf("\nTeste solução inicial\n");
  printf("Tempo de execução: %.6f segundos\n", tempo_execucao);
  printf("Função Objetivo (FO): %.2f\n", s.fo);
}

void teste_sol_1000(const char *instancia) {
  Sol s;

  ler_dados(instancia);

  clock_t inicio = clock();
  for (int i = 0; i < 1000; i++) {
    calc_custo_dist();
    ordenar_nos();

    declara_hubs(s);
    melhor_hub(s);

    heu_cons_gul(s);
  }

  for (int i = 0; i < 1000; i++) {
    calc_fo(s);
  }

  clock_t fim = clock();

  double tempo_execucao = (double)(fim - inicio) / CLOCKS_PER_SEC;

  // imprimir_sol(s);
  // arqv_sol(s);

  printf("\nTeste solução 1000\n");
  printf("Tempo de execução: %.6f segundos\n", tempo_execucao);
  printf("Função Objetivo (FO): %.2f\n", s.fo);
}