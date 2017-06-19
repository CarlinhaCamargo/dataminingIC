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
#include <cmath>
#include <cstring>
#include <algorithm>

#define TAMANHO_POPULACAO 50
#define TAMANHO_GENES     34
#define TAMANHO_BASE_TREINAMENTO 239
#define TAMANHO_BASE_TESTE 119
#define CLASSE_EXECUCAO 1 
#define MUTACAO_QTD_REAL 11
#define TOTAL_GERACOES 50
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
void geraPopulacao(vector<Individuo>& populacao, int tamanhoP);
void printIndividuo(Individuo individuo);
void printPopulacao(vector<Individuo> populacao);
void funcaoAvaliacaoInicial(Individuo *individuo);
void leBase(char *base, Fase tipo);
void crossover(Individuo pai1, Individuo pai2, Individuo *filho1, Individuo *filho2);
void mutacao(vector<Individuo>& populacao);
Individuo torneioEstocastico(vector<Individuo> populacao);
int roleta(vector<Individuo> individuo);
int torneio(vector<Individuo> populacao,int pai1,int pai2, int pai3);
bool compare(Individuo a, Individuo b);
Individuo selecionaMelhor(vector<Individuo>& populacao);
void reinsercaoElitismo(vector<Individuo>& populacao);
void execucao();


/************ VARIAVEIS GLOBAIS *****************/

vector<Individuo> baseTreinamento; //tera 2/3 da base = 244
vector<Individuo> baseTeste;       //tera 1/3 da base 122



/*
 * 
 */
int main(int argc, char** argv) {
    srand(time(NULL));
    
    execucao();
    
    
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
        printf("[%.2f|%d|%d]  ", individuo.genes[i].peso, individuo.genes[i].operador, individuo.genes[i].valor);
    }

    printf("ap = %.2f ",individuo.aptidao);
    printf("\n\n");
}
void printPopulacao(vector<Individuo> populacao){
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
void geraPopulacao(vector<Individuo>& populacao, int tamanhoP){
    
    if(populacao.size()>=0) populacao.clear();
     
    for(int i=0; i<tamanhoP; i++){
        Individuo aux = geraIndividuo();
        populacao.push_back(aux);
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

void mutacao(vector<Individuo>& populacao){
    
    int aux1, aux2,aux3,i,posPeso,posOperador,posValor;
    int pes,op,op1,val,val1,ida,ida1;
    
    int populacaoMutacaoSelecionada[MUTACAO_QTD_REAL];
    int temp;
    bool repetido = false;
    Individuo individuo;
    
   
    i=0;
    while(i<MUTACAO_QTD_REAL){
        temp = numeroRandomicoInt(0,34);
        repetido = false;
        for(int j=0;j<MUTACAO_QTD_REAL; j++){
            if(temp == populacaoMutacaoSelecionada[j]){
                repetido = true;
                break;
            }
        }
        if(!repetido){
            populacaoMutacaoSelecionada[i++] = temp;       
        }
    }
    
    for(i=0;i<MUTACAO_QTD_REAL;i++){
        individuo = populacao[populacaoMutacaoSelecionada[i]];

          // numero entre 1 e 3 (incluso ambos) sofre mutação
         posPeso = numeroRandomicoDouble(0,1);
         posOperador = numeroRandomicoInt(0,3);
         posValor = numeroRandomicoInt(0,3);
         aux1 =  numeroRandomicoInt(1,10);
         aux2 =  numeroRandomicoInt(1,10);
         aux3 =  numeroRandomicoInt(1,10);
         //printf("[%.2f|%d|%d] \n\n ", individuo.genes[posPeso].peso, individuo.genes[posMut].operador, individuo.genes[posMut].valor);
         // mutação peso
         if(aux1 <= 3)
         {
             // 1 significa que vai subtrair 0,1 e 2 significa que vai add 0,1 no peso
             pes = numeroRandomicoInt(1,2);
             if(pes == 1){
                individuo.genes[posPeso].peso = (individuo.genes[posPeso].peso - 0.1);
             }else{
                individuo.genes[posPeso].peso = (individuo.genes[posPeso].peso + 0.1);
             }
         }
         //mutação simbolo
         if(aux2 <= 3)
         {
             op = numeroRandomicoInt(1,4);
             op1 = individuo.genes[posOperador].operador;
             while(op == op1)
             {
                  op = numeroRandomicoInt(1,4);
             }
             individuo.genes[posOperador].operador = op;
         }
         // mutação para valor
         if(aux3 <= 3)
         {
             val = numeroRandomicoInt(1,3);
             val1 = individuo.genes[posValor].valor;
             while(val == val1)
             {
                  val = numeroRandomicoInt(1,4);
             }
             individuo.genes[posValor].valor = val;
         }


         //mutação idade
         if(populacaoMutacaoSelecionada[i] == 11){
            if(aux3 <= 3)
            {
                ida = numeroRandomicoInt(1,79);
                ida1 = individuo.genes[i].valor;
                while(ida == ida1)
                {
                     ida = numeroRandomicoInt(0,79);
                }
                individuo.genes[i].valor = ida;
            }
            
         }
         else if(populacaoMutacaoSelecionada[i] == 34){// mutação historico familiar

            if(aux3 <= 3)
            {
                if(individuo.genes[i].valor == 1)
                   individuo.genes[i].valor = 0;
                else
                   individuo.genes[i].valor = 1;
            }
         }
         
        funcaoAvaliacaoInicial(&individuo);
        populacao[populacaoMutacaoSelecionada[i]] = individuo;
    }
}

Individuo torneioEstocastico(vector<Individuo> populacao){
    
    int sorteio1, sorteio2, sorteio3, selecionado;
    
    sorteio1 = roleta(populacao);
    sorteio2 = roleta(populacao);
    sorteio3 = roleta(populacao);
    
    selecionado = torneio(populacao, sorteio1, sorteio2, sorteio3);
    
    return populacao[selecionado];
}

// retorna a posição do item selecionado, chamar 3x para obter os 3 itens e ver qual o melhor deles
int roleta(vector<Individuo> individuo){
    double pos[TAMANHO_GENES];
    double sorteio;
    int contador = 0;
    pos[0] = individuo[0].aptidao;
    
    for(int i=1;i<TAMANHO_POPULACAO;i++){
      individuo[i].aptidao =  individuo[i].aptidao + individuo[i -1].aptidao; 
       pos[i] = individuo[i].aptidao;
    }
    sorteio = numeroRandomicoDouble(0, int(pos[TAMANHO_GENES]));
    while(sorteio >= individuo[contador].aptidao )
    {
       contador++;
    }
    return contador;
}

// retorna o melhor elemento dos 3 que a roleta filtrou
int torneio(vector<Individuo> populacao,int pai1,int pai2, int pai3){
    if(populacao[pai1].aptidao >= populacao[pai2].aptidao && populacao[pai1].aptidao >= populacao[pai3].aptidao)
    {
        return pai1;
    }
    else if (populacao[pai2].aptidao >= populacao[pai3].aptidao && populacao[pai2].aptidao >= populacao[pai1].aptidao)
    {
        return pai2;
    }
    else {
        return pai3;
    }
}

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
    
    funcaoAvaliacaoInicial(&(*filho1));
    funcaoAvaliacaoInicial(&(*filho2));
    return; 
}


void reinsercaoElitismo(vector<Individuo> populacaoAntiga, vector<Individuo>& populacaoNova){
//    printf("reinsercao \n");
//    printPopulacao(populacaoAntiga);
//    printPopulacao(populacaoNova);
//    printf(" populacoes ");
    Individuo melhor = selecionaMelhor(populacaoAntiga);
//    printf("selecionou \n");
    
   
  
    populacaoNova.erase(populacaoNova.begin());
    populacaoNova.push_back(melhor);
}
bool compare(Individuo a,Individuo b){
    return (a.aptidao > b.aptidao);
}
Individuo selecionaMelhor(vector<Individuo> & populacao){
//    printf("selecao \n\n\n\n\n\n");
    sort(populacao.begin(), populacao.end(), compare);
//    printf("quicksort done \n");
    return *populacao.begin();
}

void execucao(){
    int geracoes = TOTAL_GERACOES;
    bool aptidaoOtima = false;
    int aptidaoOtimaIndex = -1;
    
    
    vector<Individuo> populacao;
    geraPopulacao(populacao, TAMANHO_POPULACAO); //gera a populacao inicial
    
    char base[] = "trainingBase.txt";             // le a base para a funcao de avaliacao
    leBase(base, FASE_TREINAMENTO); 
    
    for(int i=0; i<TAMANHO_POPULACAO; i++){
        funcaoAvaliacaoInicial(&populacao[i]);    //avalia a populacao inicial
    }
//    printf("************************************************AVALIACAO INICIAL ******************************* \n");
//    printPopulacao(populacao);
//    
//    
    
    //passo loop: checar se alguma aptidao == 1: se tiver retorna esse elemento e acaba execucao senao continua
    while(geracoes-- && !aptidaoOtima){  //rodar 50 geracoes ou ate encontrar regra com aptidao 1 
//        printf("execucao\n");
        for(int i=0; i<TAMANHO_POPULACAO; i++){
            if(populacao[i].aptidao == 1.0){
                aptidaoOtima = true;
                aptidaoOtimaIndex = i;
                break;
            }
        }
        
        if(aptidaoOtima){
            printf("Individuo otimo encontrado na posicao %d:\n ", aptidaoOtimaIndex);
            printIndividuo(populacao[aptidaoOtimaIndex]);
            return;
        }
//        printf("GERACAO %d \n" , geracoes);
//        printf("************************************************ANTES DA SELECAO ******************************* \n");
//        printPopulacao(populacao);
        //selecionar os elementos dois a dois para o crossover; 
        
        vector<Individuo> populacaoNova;
        Individuo filho1, filho2, pai1, pai2;
        for(int i=0; i<TAMANHO_POPULACAO; i+=2){
//            pai1 = torneioEstocastico(populacao);
//            pai2 = torneioEstocastico(populacao);
////             printf("crossover will happen\n");
//
//            printf("pai1 = \n ");
//            printIndividuo(pai1);
//            printf("pai2 = \n ");
//            printIndividuo(pai2);
//            
//            
//            crossover(pai1,pai2, &filho1, &filho2);
////             printf("crossover done\n");
//            printf("filho1 = \n ");
//            printIndividuo(filho1);
//            printf("filho2 = \n ");
//            printIndividuo(filho2);
            
            crossover(populacao[i], populacao[i+1], &filho1, &filho2);
            
            populacaoNova.push_back(filho1);
            populacaoNova.push_back(filho2);   
        }
//        printf("crossover done\n");
//        printPopulacao(populacaoNova);

        //mutar os filhos 
        mutacao(populacaoNova);
//        printf("mutacao done\n");
//        printPopulacao(populacaoNova);
//        
    
//         printf("mutacao done\n");
    
    //selecionar o melhor e retonar a nova populacao para passo loop. 
        reinsercaoElitismo(populacao,populacaoNova);
        
//         printf("reinsercao done\n");

        
//        copy(populacaoNova, populacaoNova+TAMANHO_POPULACAO, populacao.begin());
//        memcpy(&populacao, &populacaoNova, sizeof(populacao));
//        printf("################################AQUIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII##########################################\n\n\n\n");
        populacao = populacaoNova;
//        printf("################################AFTEEEEEEEEEEEEEEER##########################################\n\n\n\n");
        
        aptidaoOtima = false;
        
    }
    //geracoes acabaram, entao seleciona a melhor regra
    Individuo regraFinal = selecionaMelhor(populacao);
    printf("regra final: \n");
    printIndividuo(regraFinal);

}