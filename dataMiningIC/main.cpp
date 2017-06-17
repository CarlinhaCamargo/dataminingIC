/* 
 * File:   main.cpp
 * Author: carla
 *
 * Created on 10 de Junho de 2017, 13:37
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <vector>

#define TAMANHO_POPULACAO 50
#define TAMANHO_GENES     34
#define TAMANHO_BASE_TREINAMENTO 239
#define TAMANHO_BASE_TESTE 119
#define CLASSE_EXECUCAO 1 

using namespace std;


/************ DEFINICAO DOS TIPOS ***************/
typedef struct {
    double peso;
    int operador;
    int valor; 
}Gene;

typedef struct {
    Gene genes[TAMANHO_GENES];
    double aptidao;  
}Individuo;

/*
 * OPERADOR
 * 0 == IGUAL
 * 1 == DIFERENTE
 * 2 == MAIORIGUAL
 * 3 == MENOR
 */


enum Fase {FASE_TREINAMENTO, FASE_TESTE};


/************ DECLARACAO DAS FUNCOES ************/
double numeroRandomicoDouble(int inicio, int fim);
int numeroRandomicoInt(int inicio, int fim);
Individuo geraIndividuo();
void geraPopulacao(Individuo** populacao);
void printIndividuo(Individuo individuo);
void printPopulacao(Individuo *populacao);
void funcaoAvaliacaoInicial(Individuo *individuo);
void leBase(char *base, Fase tipo);



/************ VARIAVEIS GLOBAIS *****************/

vector<Individuo> baseTreinamento; //tera 2/3 da base = 244
vector<Individuo> baseTeste;       //tera 1/3 da base 122



/*
 * 
 */
int main(int argc, char** argv) {
    srand(time(NULL));
    
    Individuo *populacao;
    geraPopulacao(&populacao);
    //printPopulacao(populacao);
    printf("#####################################################################################");
    char base[] = "trainingBase.txt";
    leBase(base, FASE_TREINAMENTO);
    for(int i=0; i<TAMANHO_POPULACAO; i++){
        funcaoAvaliacaoInicial(&populacao[i]);
    }
    //printPopulacao(populacao);
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
//    printf("\nHistorico: %d", individuo.historico_familiar);
//    printf("\nIdade: %d", individuo.idade);
    printf("ap = %.2f ",individuo.aptidao);
    printf("\n\n");
}

void printPopulacao(Individuo *populacao){
    for(int i=0; i<TAMANHO_POPULACAO; i++){
        printIndividuo(populacao[i]);
    }
    
}
void leBase(char *base, Fase tipo){
    
    /*Na leitura da base, eu to colocando a classe do registro 
     no campo aptidao, ja que o registro em si eh um individuo*/
    
    
    int i=0, j=0; 
    int aux;
    
    std::ifstream file;
    file.open(base);
    
    if(file.is_open()){
        while(!file.eof()){
            Individuo temp;
            file >> aux;
            if (j==34){                
                temp.aptidao = aux;
                j=0;
            }else{
                temp.genes[j].valor = aux;
                j++;
            }
            if(tipo == FASE_TREINAMENTO){
                baseTreinamento.push_back(temp);
            }else if(tipo == FASE_TESTE){
                baseTeste.push_back(temp);
            }
        }
        
        file.close();
    }
}

Individuo geraIndividuo(){
    
    Individuo individuo;
    
    for(int i=0; i<TAMANHO_GENES; i++){
        if (i == 11){ //historico familiar
            individuo.genes[i].valor = numeroRandomicoInt(0,1);
        }else if(i == 34) { //idade
            individuo.genes[i].valor = numeroRandomicoInt(0,79);
        }else{
            individuo.genes[i].valor = numeroRandomicoInt(0,3);
        }
            
        
        individuo.genes[i].peso = numeroRandomicoDouble(0,1);
        individuo.genes[i].operador = numeroRandomicoInt(0,3);
        
    }
    
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



void funcaoAvaliacaoInicial(Individuo *individuo){
    
    /*vai varrer o individuo confrontando com todos os registros da base */
    int contTP=0, contTN=0, contFP=0, contFN = 0;
    double SE, SP, aptidao;
    int VI, VB, OP;
    bool C1, C2, NaoC1, NaoC2;
    
    for(int i=0; i<TAMANHO_BASE_TREINAMENTO; i++){
        for(int j=0; j<TAMANHO_GENES; j++){
            //comparar gene i da base com gene j do individuo recebido
            // atribuir o incremento no devido contador
            // verificar o caso especial no gene 11 e 34 
//            (*individuo).genes[j].
            VB = baseTreinamento[i].genes[j].valor;
            VI = (*individuo).genes[j].valor;
                     
            switch((*individuo).genes[j].operador){
                case 0:
                    C1 = (VI == VB);
                    break;
                case 1:
                    C1 = (VI != VB);
                    break;
                case 2:
                    C1 = (VI >= VB);
                    break;
                case 3:
                    C1 = (VI < VB);
                    break;
                default:
                    C1 = false;
                    break;
            }
            C2 = (baseTreinamento[i].aptidao == CLASSE_EXECUCAO);
            

            if (C1 && C2){
                contTP++;
            }else if(C1 && !C2){
                contFP++;
            }else if(!C1 && C2){
                contFN++;
            }else if(!C1 && !C2){
                contTN++;
            }else{ //erro fatal
                contTP = contTN = contFN = contFP = 0;
            }
        }
    } 
            
    SE = (contTP + 1.0) / (contTP + contFN + 1.0); //o 1 foi adicionado para ajudar na arrancada da convergencia
    SP = (contTN + 1.0) / (contTN + contFP + 1.0);
    
    
//    printf("apt = %f \n", SE*SP);
    (*individuo).aptidao = SE * SP;
    return;
}





//void mutacao(Individuo** populacao){
//    int aux1, aux2,aux3,i,posPeso,posOperador,posValor;
//    int pes,op,op1,val,val1,ida,ida1;
//    //como é 30% de mutação arredondei para 11
//    for(i=0;i<11;i++){
//          // numero entre 1 e 3 (incluso ambos) sofre mutação
//         posPeso = numeroRandomicoInt(1,34);
//         posOperador = numeroRandomicoInt(1,34);
//         posValor = numeroRandomicoInt(1,34);
//         aux1 =  numeroRandomicoInt(1,10);
//         aux2 =  numeroRandomicoInt(1,10);
//         aux3 =  numeroRandomicoInt(1,10);
//         //printf("[%.2f|%d|%d] \n\n ", individuo.genes[posPeso].peso, individuo.genes[posMut].operador, individuo.genes[posMut].valor);
//         // mutação peso
//         if(aux1 <= 3)
//         {
//             // 1 significa que vai subtrair 0,1 e 2 significa que vai add 0,1 no peso
//             pes = numeroRandomicoInt(1,2);
//             if(pes == 1)
//                individuo.genes[posPeso].peso = (individuo.genes[posPeso].peso - 0.1)
//             else
//                individuo.genes[posPeso].peso = (individuo.genes[posPeso].peso + 0.1)
//         }
//         //mutação simbolo
//         if(aux2 <= 3)
//         {
//             op = numeroRandomicoInt(1,4);
//             op1 = individuo.genes[posOperador].operador;
//             while(op == op1)
//             {
//                  op = numeroRandomicoInt(1,4);
//             }
//             individuo.genes[posOperador].operador = op;
//         }
//         // mutação para valor
//         if(aux3 <= 3)
//         {
//             val = numeroRandomicoInt(1,3);
//             val1 = individuo.genes[posValor].valor;
//             while(val == val1)
//             {
//                  val = numeroRandomicoInt(1,4);
//             }
//             individuo.genes[posValor].valor = val;
//         }
//         //mutação idade
//
//         if(aux3 <= 3)
//         {
//             ida = numeroRandomicoInt(1,79);
//             ida1 = individuo.idade;
//             while(ida == ida1)
//             {
//                  ida = numeroRandomicoInt(1,79);
//             }
//             individuo.idade = ida;
//         }
//         // mutação historico familiar
//         if(aux3 <= 3)
//         {
//
//             if(individuo.historico_familiar == 1)
//                individuo.historico_familiar = 0;
//             else
//                individuo.historico_familiar = 1;
//         }
//         //printf("[%.2f|%d|%d] \n\n ", individuo.genes[posMut].peso, individuo.genes[posMut].operador, individuo.genes[posMut].valor);
//    }
//
//}
//
//
//
//void roleta_funcionamento(Regra regras[50],float pos[50],float *somaap){ //180 100
//    int i;
//    *somaap=0;
//    //soma dos valores da avaliação
//    for(i=0;i<50;i++){
//        *somaap = *somaap + regras[i].aptidao;
//    }
//    //cria o vetor com a soma das avaliações para escolher a posição escolhida da roleta
//    pos[0]=regras[0].aptidao;
//    for(i=1;i<50;i++){
//        pos[i]=pos[i-1]+regras[i].aptidao;
//    }
//}
//
//int roleta(Regra regras[50],float somaap,float pos[50]){
//    float b;
//    int i;
//    int pai;
//    // a recebe o valor aleatorio entre 0 a somaap;
//    
//    float a = somaap;   
//    b=(((float)rand()/(float)(RAND_MAX)) * a);
//    
//    //percorre o vetor de posiçoes para achar a posição escolhida
//    for(i=0;i<50;i++){
//        if(b<=pos[i]){
//            pai=i;
//            i=50;
//        }
//    }
//    //printf("indice-%d\n",pai);
//    return pai;
//}



void crossover(Individuo pai1, Individuo pai2, Individuo *filho1, Individuo *filho2){
    int ponto1, ponto2, maiorPonto, menorPonto;
    
    ponto1 = numeroRandomicoInt(1, TAMANHO_GENES);
    ponto2 = numeroRandomicoInt(1, TAMANHO_GENES);
    while(ponto1==ponto2){
        ponto2 = numeroRandomicoInt(0, TAMANHO_GENES);
    }    
    if(ponto1>ponto2){
        maiorPonto = ponto1;
        menorPonto = ponto2;
    }else{
        maiorPonto = ponto2;
        menorPonto = ponto1;
    }    
    
    for (int i=0; i<TAMANHO_GENES;i++){
        if(i<menorPonto || i>=maiorPonto){
            (*filho1).genes[i] = pai1.genes[i];
            (*filho2).genes[i] = pai2.genes[i];
        }else if( i>=menorPonto && i<maiorPonto){
            (*filho1).genes[i] = pai2.genes[i];
            (*filho2).genes[i] =  pai1.genes[i];
        }else{ //erro fatal
            return;
        }    
    }
    return; 
}