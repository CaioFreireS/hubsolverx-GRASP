#ifndef HUBSOLVER_H_INCLUDED
#define HUBSOLVER_H_INCLUDED

#include <algorithm>
#include <chrono>
#include <stdio.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory.h>
#include <sstream>
#include <time.h>
#include <vector>
#include <random>
#include <ctime>
#include <cstdlib>

using namespace std;

#define MAX_NOS 200
#define MAX_HUBS 50
#define ALPHA 0.75
#define BETA 0.35
#define DELTA 0.3
#define DISTANCIA(p1, p2)                                                      \
  (sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y)))

// Estruturas
typedef struct No {
  int id;
  double x;
  double y;
} No;

typedef struct Caminho {
  int o, h1, h2, ds;
  double custo;
} Caminho;

typedef struct tSolucao {
  double fo;
  int vet_hubs[MAX_HUBS];
  Caminho cam[MAX_NOS * MAX_NOS];
} Sol;

// Dados de entrada
int num_nos;
int num_hubs;
int num_caminhos; 
int o, ds;
int vet_ind_no[MAX_NOS];
int no_hub[MAX_NOS];
No vet_nos[MAX_NOS];
double vet_med_custo[MAX_NOS];
double mat_custo_total[MAX_NOS][MAX_NOS];
double mat_custo[MAX_NOS][MAX_NOS];

// Funções
//parte1
void ler_dados(const char *arq);
void calc_custo_dist();
void ordenar_nos();
void declara_hubs(Sol &s);
void melhor_hub(Sol &s);
void heu_cons_gul(Sol &s);
void calc_fo(Sol &s);
void imprimir_sol(Sol &s);
void arqv_sol(Sol &s);
void ler_sol(const char *nome_arquivo, Sol &s);
void clonar_sol(const Sol &s1, Sol &s2);
void teste_sol_i(const char* instancia);
void teste_sol_1000(const char* instancia);

//parte 2
void grasp(Sol &melhor_sol, double tempo_limite);
void construir_solucao(Sol &s);
void busca_local(Sol &s, Sol &melhor_sol);

#endif // HUBSOLVER_H_INCLUDED
