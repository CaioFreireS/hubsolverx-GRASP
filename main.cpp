#include "hubsolver.h"

// #define parte1test
// #define parte2test
// #define teste
// #define parte1
#define parte2


using namespace std;
int main(int arqc, char const *argv[]) {
  num_hubs = 50;
  const char *instancia = (arqc > 2) ? argv[2] : "inst200.txt";
  //const char *sol_oti = (arqc > 3) ? argv[3] : "solucaoOtima.txt";

  ler_dados(instancia);
  #ifdef parte1test
    
    Sol s;

    heu_cons_gul(s);

    calc_fo(s);

    // imprimir_sol(s);

    arqv_sol(s,"solucaoOtima.txt");
    // Sol s2;
  
    // clonar_sol(s,s2);
    // imprimir_sol(s2);
    
  #endif

  #ifdef parte1
    teste_sol_i(instancia);
    teste_sol_1000(instancia);
  #endif


  #ifdef parte2test
    Sol melhor_sol;
    ler_dados(instancia);
    tratar_dados(melhor_sol);

    ler_sol("solucaoOtima.txt", melhor_sol);

    printf("Melhor fo (pré-grasp): %.2f\n", melhor_sol.fo);

    const char *inst = "inst200.txt";
    double tempo_limite;
    grasp(melhor_sol, 30);

    printf("\nMelhor FO encontrada: %.2f\n", melhor_sol.fo);
  #endif

  #ifdef parte2
    teste_grasp();
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

void arqv_sol(Sol &s, const char *nome_arquivo) {
  FILE *arq;
  arq = fopen(nome_arquivo, "w");

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

void declara_hubs(Sol &s) {
  s.vet_hubs[0] = vet_ind_no[0];
  int count = 1;

  for (int i = 1; i < num_nos && count < num_hubs; i++) {
      bool longe = true;
      for (int j = 0; j < count; j++) {
          if (mat_custo[vet_ind_no[i]][s.vet_hubs[j]] < (DELTA * 20000)) {
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

void tratar_dados(Sol &s) {
  calc_custo_dist();
  ordenar_nos();
  declara_hubs(s);
  melhor_hub(s);
}

void heu_cons_gul(Sol &s) {
  tratar_dados(s);
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
  arqv_sol(s, "solucao1000.txt");

  printf("\nTeste solução 1000\n");
  printf("Tempo de execução sol. inicial: %.6f segundos\n", tempo_execucao);
  printf("Tempo de execução sol. calc fo: %.6f segundos\n", tempo_execucao2);
  printf("Função Objetivo (FO): %.2f\n", s.fo);
}


//parte2
void grasp(Sol &melhor_sol, double tempo_limite) {
    clock_t inicio = clock();
    double tempo_decorrido;

    Sol s;

    for (int it = 0;; it++) {
        tempo_decorrido = (double)(clock() - inicio) / CLOCKS_PER_SEC;
        if (tempo_decorrido >= tempo_limite) break;

        LRC(s);
        busca_local(s, melhor_sol);
        calc_fo(s);

        if (s.fo < melhor_sol.fo) {
            melhor_sol = s;
        }

        // printf("\nIteração %d - FO: %.2f", it + 1, s.fo);
    }
}

void seleciona_hubs(double limite) {
  std::srand(std::time(0));
  std::vector<int> candidatos_maiores, candidatos_menores;

  for (int i = 0; i < num_nos; i++) {
      if (vet_med_custo[i] >= limite) {
          candidatos_maiores.push_back(i);
      } else {
          candidatos_menores.push_back(i);
      }
  }

  std::random_shuffle(candidatos_maiores.begin(), candidatos_maiores.end());
  std::random_shuffle(candidatos_menores.begin(), candidatos_menores.end());

  int j = 0;

  for (int i = 0; i < (int)candidatos_maiores.size() && j < num_hubs * 2; i++) {
      vet_ind_no[j++] = candidatos_maiores[i];
  }

  for (int i = 0; i < (int)candidatos_menores.size() && j < num_hubs * 2; i++) {
      vet_ind_no[j++] = candidatos_menores[i];
  }
}

void LRC(Sol &s) {
  calc_custo_dist();
  ordenar_nos();

  double cmin, cmax, limite;
  cmin = *min_element(vet_med_custo, vet_med_custo + num_nos);
  cmax = *max_element(vet_med_custo, vet_med_custo + num_nos);

  limite = cmin + OMEGA * (cmax - cmin);

  seleciona_hubs(limite);

  for (int i = 0; i < num_hubs; i++) {
      s.vet_hubs[i] = vet_ind_no[i];
  }

  for (int i = 0; i < num_nos; i++) {
    bool eh_hub = false;
    for (int j = 0; j < num_hubs; j++) {
        if (s.vet_hubs[j] == i) {
            eh_hub = true;
            break;
        }
    }
    if (eh_hub) {
        no_hub[i] = i;
        continue;
    }
  }

  calc_fo(s);
  arqv_sol(s, "solucaoLRC.txt");

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

void resultado(Res &r,const char* instancia, int p, double tempo) {
  Sol s;
  ler_dados(instancia);
  heu_cons_gul(s);
  calc_fo(s);
  
  clock_t inicio,fim;
  double tempo_execucao,melhor_tempo=numeric_limits<double>::max(),media_tempo,soma_tempo = 0,melhor_fo=numeric_limits<double>::max(),media_fo,soma_fo = 0, desvio;
  num_hubs = p;

  for(int i=0;i<3;i++){
    inicio = clock();
    grasp(s, tempo);
    fim = clock();
    
    if (s.fo<melhor_fo)
      melhor_fo = s.fo;

    soma_fo += s.fo;

    tempo_execucao = (double)(fim - inicio) / CLOCKS_PER_SEC;

    if (tempo_execucao<melhor_tempo)
      melhor_tempo = tempo_execucao;

    soma_tempo += tempo_execucao;
  }

  media_tempo = soma_tempo/3;
  media_fo = soma_fo/3;

  printf("media fo: %.2f\n",media_fo);
  desvio=((media_fo-melhor_fo)/melhor_fo)*100;

  r.melhor_fo = melhor_fo;
  r.media_fo = media_fo;
  
  printf("media fo: %.2f\n",r.media_fo);
  r.melhor_tempo = melhor_tempo;
  r.media_tempo = media_tempo;
  r.desvio = desvio;
}

void teste_grasp() {
  double min=1;

  Res r5_3;
  resultado(r5_3,"inst5.txt",3,min);

  Res r10_2;
  resultado(r10_2,"inst10.txt",2,min);
  Res r10_3;
  resultado(r10_3,"inst10.txt",3,min);

  Res r20_2;
  resultado(r20_2,"inst20.txt",2,min);
  Res r20_3;
  resultado(r20_3,"inst20.txt",3,min);
  Res r20_5;
  resultado(r20_5,"inst20.txt",5,min);

  Res r25_2;
  resultado(r25_2,"inst25.txt",2,min);
  Res r25_4;
  resultado(r25_4,"inst25.txt",4,min);
  Res r25_5;
  resultado(r25_5,"inst25.txt",5,min);

  Res r40_2;
  resultado(r40_2,"inst40.txt",2,min);
  Res r40_5;
  resultado(r40_5,"inst40.txt",5,min);
  Res r40_10;
  resultado(r40_10,"inst40.txt",10,min);
  
  Res r50_2;
  resultado(r50_2,"inst50.txt",2,min);
  Res r50_3;
  resultado(r50_3,"inst50.txt",3,min);
  Res r50_5;
  resultado(r50_5,"inst50.txt",5,min);
  Res r50_10;
  resultado(r50_10,"inst50.txt",10,min);

  Res r100_4;
  resultado(r100_4,"inst100.txt",4,min*3);
  Res r100_10;
  resultado(r100_10,"inst100.txt",10,min*3);
  Res r100_20;
  resultado(r100_20,"inst100.txt",20,min*3);
  Res r100_40;
  resultado(r100_10,"inst100.txt",40,min*3);
  
  Res r200_3;
  resultado(r200_3,"inst200.txt",3,min*3);
  Res r200_5;
  resultado(r200_5,"inst200.txt",5,min*3);
  Res r200_10;
  resultado(r200_10,"inst200.txt",10,min*3);
  Res r200_30;
  resultado(r200_30,"inst200.txt",30,min*3);
  Res r200_50;
  resultado(r200_50,"inst200.txt",50,min*3);

  double soma_melhor_fo = r5_3.melhor_fo + r10_2.melhor_fo + r10_3.melhor_fo + r20_2.melhor_fo + r20_3.melhor_fo + r20_5.melhor_fo + r25_2.melhor_fo + r25_4.melhor_fo + r25_5.melhor_fo + r40_2.melhor_fo + r40_5.melhor_fo + r40_10.melhor_fo + r50_2.melhor_fo + r50_3.melhor_fo + r50_5.melhor_fo + r50_10.melhor_fo + r100_4.melhor_fo + r100_10.melhor_fo + r100_20.melhor_fo + r100_40.melhor_fo + r200_3.melhor_fo + r200_5.melhor_fo + r200_10.melhor_fo + r200_30.melhor_fo + r200_50.melhor_fo;
  double soma_media_fo = r5_3.media_fo + r10_2.media_fo + r10_3.media_fo + r20_2.media_fo + r20_3.media_fo + r20_5.media_fo + r25_2.media_fo + r25_4.media_fo + r25_5.media_fo + r40_2.media_fo + r40_5.media_fo + r40_10.media_fo + r50_2.media_fo + r50_3.media_fo + r50_5.media_fo + r50_10.media_fo + r100_4.media_fo + r100_10.media_fo + r100_20.media_fo + r100_40.media_fo + r200_3.media_fo + r200_5.media_fo + r200_10.media_fo + r200_30.media_fo + r200_50.media_fo;
  double soma_melhor_tempo = r5_3.melhor_tempo + r10_2.melhor_tempo + r10_3.melhor_tempo + r20_2.melhor_tempo + r20_3.melhor_tempo + r20_5.melhor_tempo + r25_2.melhor_tempo + r25_4.melhor_tempo + r25_5.melhor_tempo + r40_2.melhor_tempo + r40_5.melhor_tempo + r40_10.melhor_tempo + r50_2.melhor_tempo + r50_3.melhor_tempo + r50_5.melhor_tempo + r50_10.melhor_tempo + r100_4.melhor_tempo + r100_10.melhor_tempo + r100_20.melhor_tempo + r100_40.melhor_tempo + r200_3.melhor_tempo + r200_5.melhor_tempo + r200_10.melhor_tempo + r200_30.melhor_tempo + r200_50.melhor_tempo;
  double soma_media_tempo = r5_3.media_tempo + r10_2.media_tempo + r10_3.media_tempo + r20_2.media_tempo + r20_3.media_tempo + r20_5.media_tempo + r25_2.media_tempo + r25_4.media_tempo + r25_5.media_tempo + r40_2.media_tempo + r40_5.media_tempo + r40_10.media_tempo + r50_2.media_tempo + r50_3.media_tempo + r50_5.media_tempo + r50_10.media_tempo + r100_4.media_tempo + r100_10.media_tempo + r100_20.media_tempo + r100_40.media_tempo + r200_3.media_tempo + r200_5.media_tempo + r200_10.media_tempo + r200_30.media_tempo + r200_50.media_tempo;
  double soma_desvio = r5_3.desvio + r10_2.desvio + r10_3.desvio + r20_2.desvio + r20_3.desvio + r20_5.desvio + r25_2.desvio + r25_4.desvio + r25_5.desvio + r40_2.desvio + r40_5.desvio + r40_10.desvio + r50_2.desvio + r50_3.desvio + r50_5.desvio + r50_10.desvio + r100_4.desvio + r100_10.desvio + r100_20.desvio + r100_40.desvio + r200_3.desvio + r200_5.desvio + r200_10.desvio + r200_30.desvio + r200_50.desvio;
  
  double media_melhor_fo = soma_melhor_fo/25;
  double media_media_fo = soma_media_fo/25;
  double media_melhor_tempo = soma_melhor_tempo/25;
  double media_media_tempo = soma_media_tempo/25;
  double media_desvio = soma_desvio/25;
  
  FILE *arq;
  arq = fopen("Resultados.txt", "w");

  fprintf(arq, "Instância || p || Melhor FO || FO Media || Desvio || Tempo Medio(seg.) || Melhor Tempo(seg.)\n");
  fprintf(arq, "inst5.txt || 3 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r5_3.melhor_fo, r5_3.media_fo, r5_3.desvio, r5_3.media_tempo, r5_3.melhor_tempo);
  fprintf(arq, "inst10.txt || 2 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r10_2.melhor_fo, r10_2.media_fo, r10_2.desvio, r10_2.media_tempo, r10_2.melhor_tempo);
  fprintf(arq, "inst10.txt || 3 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r10_3.melhor_fo, r10_3.media_fo, r10_3.desvio, r10_3.media_tempo, r10_3.melhor_tempo);
  fprintf(arq, "inst20.txt || 2 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r20_2.melhor_fo, r20_2.media_fo, r20_2.desvio, r20_2.media_tempo, r20_2.melhor_tempo);
  fprintf(arq, "inst20.txt || 3 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r20_3.melhor_fo, r20_3.media_fo, r20_3.desvio, r20_3.media_tempo, r20_3.melhor_tempo);
  fprintf(arq, "inst20.txt || 5 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r20_5.melhor_fo, r20_5.media_fo, r20_5.desvio, r20_5.media_tempo, r20_5.melhor_tempo);
  fprintf(arq, "inst25.txt || 2 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r25_2.melhor_fo, r25_2.media_fo, r25_2.desvio, r25_2.media_tempo, r25_2.melhor_tempo);
  fprintf(arq, "inst25.txt || 4 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r25_4.melhor_fo, r25_4.media_fo, r25_4.desvio, r25_4.media_tempo, r25_4.melhor_tempo);
  fprintf(arq, "inst25.txt || 5 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r25_5.melhor_fo, r25_5.media_fo, r25_5.desvio, r25_5.media_tempo, r25_5.melhor_tempo);
  fprintf(arq, "inst40.txt || 2 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r40_2.melhor_fo, r40_2.media_fo, r40_2.desvio, r40_2.media_tempo, r40_2.melhor_tempo);
  fprintf(arq, "inst40.txt || 5 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r40_5.melhor_fo, r40_5.media_fo, r40_5.desvio, r40_5.media_tempo, r40_5.melhor_tempo);
  fprintf(arq, "inst40.txt || 10 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r40_10.melhor_fo, r40_10.media_fo, r40_10.desvio, r40_10.media_tempo, r40_10.melhor_tempo);
  fprintf(arq, "inst50.txt || 2 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r50_2.melhor_fo, r50_2.media_fo, r50_2.desvio, r50_2.media_tempo, r50_2.melhor_tempo);
  fprintf(arq, "inst50.txt || 3 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r50_3.melhor_fo, r50_3.media_fo, r50_3.desvio, r50_3.media_tempo, r50_3.melhor_tempo);
  fprintf(arq, "inst50.txt || 5 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r50_5.melhor_fo, r50_5.media_fo, r50_5.desvio, r50_5.media_tempo, r50_5.melhor_tempo);
  fprintf(arq, "inst50.txt || 10 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r50_10.melhor_fo, r50_10.media_fo, r50_10.desvio, r50_10.media_tempo, r50_10.melhor_tempo);
  fprintf(arq, "inst100.txt || 4 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r100_4.melhor_fo, r100_4.media_fo, r100_4.desvio, r100_4.media_tempo, r100_4.melhor_tempo);
  fprintf(arq, "inst100.txt || 10 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r100_10.melhor_fo, r100_10.media_fo, r100_10.desvio, r100_10.media_tempo, r100_10.melhor_tempo);
  fprintf(arq, "inst100.txt || 20 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r100_20.melhor_fo, r100_20.media_fo, r100_20.desvio, r100_20.media_tempo, r100_20.melhor_tempo);
  fprintf(arq, "inst100.txt || 40 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r100_40.melhor_fo, r100_40.media_fo, r100_40.desvio, r100_40.media_tempo, r100_40.melhor_tempo);
  fprintf(arq, "inst200.txt || 3 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r200_3.melhor_fo, r200_3.media_fo, r200_3.desvio, r200_3.media_tempo, r200_3.melhor_tempo);
  fprintf(arq, "inst200.txt || 5 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r200_5.melhor_fo, r200_5.media_fo, r200_5.desvio, r200_5.media_tempo, r200_5.melhor_tempo);
  fprintf(arq, "inst200.txt || 10 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r200_10.melhor_fo, r200_10.media_fo, r200_10.desvio, r200_10.media_tempo, r200_10.melhor_tempo);
  fprintf(arq, "inst200.txt || 30 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r200_30.melhor_fo, r200_30.media_fo, r200_30.desvio, r200_30.media_tempo, r200_30.melhor_tempo);
  fprintf(arq, "inst200.txt || 50 || %.2f || %.2f || %.2f || %.6f || %.6f\n", r200_50.melhor_fo, r200_50.media_fo, r200_50.desvio, r200_50.media_tempo, r200_50.melhor_tempo);
  fprintf(arq, "Media || || %.2f || %.2f || %.2f || %.6f || %.6f\n", media_melhor_fo, media_media_fo, media_desvio, media_media_tempo, media_melhor_tempo);
  fclose(arq);
}