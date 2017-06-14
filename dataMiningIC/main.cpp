/* 
 * File:   main.cpp
 * Author: carla
 *
 * Created on 10 de Junho de 2017, 13:37
 */

#include <cstdlib>
#include <stdio.h> 
#include <stdlib.h>
#include <time.h> 

#define TAMANHO_POPULACAO 50
#define TAMANHO_GENES     32



using namespace std;

//typedef enum {
//    MAIOR_IGUAL, 
//    MENOR, 
//    IGUAL, 
//    DIFERENTE
//}OPERADOR_TYPE;


typedef struct {
    double peso;
    int operador;
    int valor; 
}Gene;

typedef struct {
    Gene genes[TAMANHO_GENES];
    int historico_familiar;
    int idade;
}Individuo;
/************ DECLARACAO DAS FUNCOES ************/
double numeroRandomicoDouble(int inicio, int fim);
int numeroRandomicoInt(int inicio, int fim);
Individuo geraIndividuo();
void geraPopulacao(Individuo** populacao);
void printIndividuo(Individuo individuo);
void printPopulacao(Individuo *populacao);

/*
 * 
 */
int main(int argc, char** argv) {
    srand(time(NULL));
    Individuo *populacao;
    geraPopulacao(&populacao);
    printPopulacao(populacao);
    
    return 0;
}

double numeroRandomicoDouble(int inicio, int fim){
    fim = fim*100;
    int num = rand()%(fim+1-inicio)+inicio;
    return num/100.0;
    
}
int numeroRandomicoInt(int inicio, int fim){
    return rand()%(fim+1-inicio)+inicio;
}

void printIndividuo(Individuo individuo){
    for(int i=0; i<TAMANHO_GENES; i++){
//        printf("Gene %d:\n", i);
        printf("[%.2f|%d|%d]  ", individuo.genes[i].peso, individuo.genes[i].operador, individuo.genes[i].valor);
    }
    printf("\nHistorico: %d", individuo.historico_familiar);
    printf("\nIdade: %d", individuo.idade);
    printf("\n\n");
}

void printPopulacao(Individuo *populacao){
    for(int i=0; i<TAMANHO_POPULACAO; i++){
        printIndividuo(populacao[i]);
    }
    
}


Individuo geraIndividuo(){
    
    Individuo individuo;
    
    for(int i=0; i<32; i++){
        individuo.genes[i].peso = numeroRandomicoDouble(0,1);
        individuo.genes[i].operador = numeroRandomicoInt(0,3);
        individuo.genes[i].valor = numeroRandomicoInt(0,3);
    }
    individuo.historico_familiar = numeroRandomicoInt(0,1);
    individuo.idade = numeroRandomicoInt(0,79);
    
    return individuo;   
}
/* a funcao ira modificar o conteudo de um vetor de individuos. 
 * chamada do metodo deve seguir o formato geraPopulacao(&Individuo[]) */
void geraPopulacao(Individuo** populacao){
    *populacao = (Individuo *) malloc(TAMANHO_POPULACAO *(sizeof(Individuo)));
    
    if (*populacao == NULL){
        return;
    }
    for(int i=0; i<TAMANHO_POPULACAO; i++){
        (*populacao)[i] = geraIndividuo();
    }
    
    return; 
}


