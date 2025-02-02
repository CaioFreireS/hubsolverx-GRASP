#ifndef HUBGRASP_H_INCLUDED
#define HUBGRASP_H_INCLUDED

#define MAX_NOS 200
#define MAX_HUBS 50

//deve ser transformado em parametro dps
#define num_hubs 2

//estruturas 
typedef struct No{
  double x;
  double y;
}No;

typedef struct tSolucao{
  double fo;
  int vet_sol[MAX_NOS];
  int vet_hubs[MAX_HUBS];
}Sol;


//dados de entrada
int num_nos;
//int num_hubs;
int vet_ind_no[MAX_NOS];
No vet_nos[MAX_NOS];
double vet_med_custo[MAX_NOS];


//funcoes
void ler_dados(const char* arq);
void calc_custo_dist();
void ordenar_nos();
void heu_cons_gul(Sol& s);
void calc_fo(Sol& s);
void imprimir_sol(Sol& s);


#endif  //HUBGRASP_H_INCLUDED