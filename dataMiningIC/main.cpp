/* 
 * File:   main.cpp
 * Author: carla
 *
 * Created on 10 de Junho de 2017, 13:37
 */

#include <cstdlib>

using namespace std;
srand(time(NULL));

typedef enum {
    MAIOR_IGUAL, 
    MENOR, 
    IGUAL, 
    DIFERENTE
}OPERADOR_TYPE;


typedef struct {
    double peso;
    OPERADOR_TYPE operador;
    int valor; 
}Gene;

typedef struct {
    Gene genes[32];
    int historico_familiar;
    int idade;
}Individuo;

double numeroRandomicoDouble(int inicio, int fim);
int numeroRandomicoInt(int inicio, int fim);
Individuo generatePopulation();




/*
 * 
 */
int main(int argc, char** argv) {

    
    
    return 0;
}

double numeroRandomicoDouble(int inicio, int fim){
    fim = fim*100;
    int num = rand()%(fim+1-inicio)+inicio;
    return num/100.0;
    
}
int numeroRandomicoInt(int inicio, int fim){
    return num = rand()%(fim+1-inicio)+inicio;
}

Individuo generatePopulation(){
    
    Individuo individuo;
    
    for(int i=0; i<32;i++){
        individuo.genes[i].peso = numeroRandomicoDouble(0,1);
        individuo.genes[i].operador
    }
    
    
    
}
