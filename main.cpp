#include "hubsolver.h"

#define parte1
//#define parte2
#define teste
#define declara1
// #define declara2

using namespace std;
int main(int arqc, char const *argv[]) {
  num_hubs = 50;
  const char *instancia = (arqc > 2) ? argv[2] : "inst200.txt";
  //const char *sol_oti = (arqc > 3) ? argv[3] : "solucaoOtima.txt";

  ler_dados(instancia);

  #ifdef parte1
    
    Sol s;

    heu_cons_gul(s);

    calc_fo(s);

    // imprimir_sol(s);

    arqv_sol(s);
  
    // teste_sol_i(instancia);
    // teste_sol_1000(instancia);
  #endif
  // Sol s2;

  // clonar_sol(s,s2);
  // imprimir_sol(s2);

  #ifdef teste 
  #endif

  #ifdef parte2
    Sol melhor_sol;

    ler_sol("solucaoOtima.txt", melhor_sol);

    printf("Melhor fo (pré-grasp): %.2f\n", melhor_sol.fo);

    double tempo_limite;
    grasp(melhor_sol, 30);

    printf("\nMelhor FO encontrada: %.2f\n", melhor_sol.fo);
  #endif

  return 0;
}

//parte1
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
  if (!arq) {
      perror("Erro ao abrir o arquivo");
      return;
  }

  int scan1=fscanf(arq, "n: %d p: %d", &num_nos, &num_hubs);
  if (scan1 != 2) {
      printf("Erro: formato inválido ao ler 'n' e 'p'.\n");
      fclose(arq);
      return;
  }

  int scan2=fscanf(arq, " FO: %lf", &s.fo);
  if (scan2 != 1) {
      printf("Erro: formato inválido ao ler a função objetivo.\n");
      fclose(arq);
      return;
  }

  int scan3 = fscanf(arq, " HUBS [%d", &s.vet_hubs[0]);
  no_hub[0] = s.vet_hubs[0];
  if (scan3 != 1) {
    printf("Erro: formato inválido ao ler hubs.\n");
    fclose(arq);
    return;
  }

  for (int i = 1; i < num_hubs; i++) {
      int scan4=fscanf(arq, ",%d", &s.vet_hubs[i]);
      no_hub[i] = s.vet_hubs[i];
      if (scan4 != 1) {
        printf("Erro: formato inválido ao ler hubs na posição %d.\n", i);
        fclose(arq);
        return;
      }
  }

  if (fscanf(arq, " ]") != 0) { 
      printf("Erro: fechamento incorreto da lista de hubs.\n");
      fclose(arq);
      return;
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

  double dist;
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
  for (int i = 0; i < num_nos; i++)
    vet_ind_no[i] = i;

  for (int i = 0; i < num_nos - 1; i++) {
    for (int j = 0; j < num_nos - i - 1; j++) {
      if (vet_med_custo[j] > vet_med_custo[j + 1]) {
        swap(vet_med_custo[j], vet_med_custo[j + 1]);
        swap(vet_ind_no[j], vet_ind_no[j + 1]);
      }
    }
  }
}

#ifdef declara1
  void declara_hubs(Sol &s) {
    s.vet_hubs[0] = vet_ind_no[0];
    int count = 1;

    for (int i = 1; i < num_nos && count < num_hubs; i++) {
        bool longe = true;
        for (int j = 0; j < count; j++) {
            if (mat_custo[vet_ind_no[i]][s.vet_hubs[j]] < (OMEGA * 20000)) {
                longe = false;
                break;
            }
        }
        if (longe) {
            s.vet_hubs[count++] = vet_ind_no[i];
        }
    }

    for (int i = count; i < num_hubs; i++) {
        s.vet_hubs[i] = vet_ind_no[i];
    }
  }
#endif

void melhor_hub(Sol &s) {
  double dist, dist_min;
  int melhor_h,hub;
  for (int i = 0; i < num_nos; i++) {
      dist_min = numeric_limits<double>::max();
      melhor_h = -1;

      for (int j = 0; j < num_hubs; j++) {
          hub = s.vet_hubs[j];
          dist = mat_custo[i][hub];

          if (dist < dist_min) {  
              dist_min = dist;
              melhor_h = hub;
          }
      }
      no_hub[i] = melhor_h;
  }
}

#ifdef declara2
void declara_hubs(Sol &s) {
  vector<bool> selecionado(num_nos, false);
  s.vet_hubs[0] = vet_ind_no[0];
  selecionado[vet_ind_no[0]] = true;
  int count = 1;

  for (int i = 1; i < num_nos && count < num_hubs; i++) {
      int melhor_no = -1;
      double melhor_distancia = -1;

      for (int j = 0; j < num_nos; j++) {
          if (!selecionado[j]) {
              double distancia_minima = numeric_limits<double>::max();
              for (int k = 0; k < count; k++) {
                  distancia_minima = min(distancia_minima, mat_custo[j][s.vet_hubs[k]]);
              }
              if (distancia_minima > melhor_distancia) {
                  melhor_distancia = distancia_minima;
                  melhor_no = j;
              }
          }
      }

      if (melhor_no != -1) {
          s.vet_hubs[count++] = melhor_no;
          selecionado[melhor_no] = true;
      }
  }

  for (int i = count; i < num_hubs; i++) {
      s.vet_hubs[i] = vet_ind_no[i];
  }
}
#endif

void heu_cons_gul(Sol &s) {
    calc_custo_dist();
    ordenar_nos();
    declara_hubs(s);

    #ifdef declara1
      melhor_hub(s);
    #endif

    for (int i = 0; i < num_nos; i++) {
        for (int k = 0; k < num_nos; k++) {
            int h1 = no_hub[i];
            int h2 = no_hub[k];

            int cam = i * num_nos + k;
            s.cam[cam].o = i;
            s.cam[cam].h1 = h1;
            s.cam[cam].h2 = h2;
            s.cam[cam].ds = k;

        }
    }

}

void calc_fo(Sol &s) {
  for (int i = 0; i < num_nos; i++) {
    for (int k = 0; k < num_nos; k++) {
        int h1 = no_hub[i];
        int h2 = no_hub[k];

        double o_h1 = BETA * mat_custo[i][h1];
        double h1_h2 = ALPHA * mat_custo[h1][h2];
        double h2_ds = LAMBDA * mat_custo[h2][k];

        if (i == k) h1_h2 = 0;

        double custo = o_h1 + h1_h2 + h2_ds;

        int cam = i * num_nos + k;
        s.cam[cam].custo = custo;
      }
    }
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
    heu_cons_gul(s);
  }
  clock_t fim = clock();
  
  clock_t inicio2 = clock();
  for (int i = 0; i < 1000; i++) {
    calc_fo(s);
  }
  clock_t fim2 = clock();

  double tempo_execucao = (double)(fim - inicio) / CLOCKS_PER_SEC;

  double tempo_execucao2 = (double)(fim2 - inicio2) / CLOCKS_PER_SEC;

  // imprimir_sol(s);
  arqv_sol(s);

  printf("\nTeste solução 1000\n");
  printf("Tempo de execução sol. inicial: %.6f segundos\n", tempo_execucao);
  printf("Tempo de execução sol. calc fo: %.6f segundos\n", tempo_execucao2);
  printf("Função Objetivo (FO): %.2f\n", s.fo);
}


//parte2
void grasp(Sol &melhor_sol, double tempo_limite) {
    clock_t inicio = clock();
    double tempo_decorrido;

    for (int it = 0;; it++) {
        tempo_decorrido = (double)(clock() - inicio) / CLOCKS_PER_SEC;
        if (tempo_decorrido >= tempo_limite) break;

        Sol s;
        construir_solucao(s);
        busca_local(s, melhor_sol);
        calc_fo(s);

        if (s.fo < melhor_sol.fo) {
            melhor_sol = s;
        }

        printf("\nIteração %d - FO: %.2f", it + 1, s.fo);
    }
}

void construir_solucao(Sol &s) {
  // Inicializa os candidatos com os índices dos nós ordenados
  vector<int> candidatos(num_nos);
  iota(candidatos.begin(), candidatos.end(), 0);

  // Seleciona hubs de forma balanceada, considerando a média de custo
  for (int i = 0; i < num_hubs; i++) {
      int limite_rcl = max(1, (int)(DELTA * candidatos.size()));
      int idx = rand() % limite_rcl;

      s.vet_hubs[i] = vet_ind_no[idx];
      candidatos.erase(candidatos.begin() + idx);
  }

  // Associa cada nó ao hub mais próximo
  for (int i = 0; i < num_nos; i++) {
      double dist_min = numeric_limits<double>::max();
      int melhor_hub = -1;

      for (int j = 0; j < num_hubs; j++) {
          int hub = s.vet_hubs[j];
          double dist = mat_custo[i][hub];

          if (dist < dist_min) {
              dist_min = dist;
              melhor_hub = hub;
          }
      }

      no_hub[i] = melhor_hub;
  }
}

void busca_local(Sol &s, Sol &melhor_sol) {
    bool houve_melhora;

    do {
        houve_melhora = false;
        for (int i = 0; i < num_hubs; i++) {
            for (int j = 0; j < num_nos; j++) {
                if (find(s.vet_hubs, s.vet_hubs + num_hubs, j) == s.vet_hubs + num_hubs) {
                    int hub_antigo = s.vet_hubs[i];
                    s.vet_hubs[i] = j;

                    calc_fo(s);

                    if (s.fo < melhor_sol.fo) {
                        houve_melhora = true;
                        melhor_sol = s;
                    } else {
                        s.vet_hubs[i] = hub_antigo; 
                    }
                }
            }
        }
    } while (houve_melhora);
}
