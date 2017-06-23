/* 
 * File:   main.cpp
 * Author: carla
 *
 * Created on 10 de Junho de 2017, 13:37
 */


#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <chrono>
#include <random>

#define TAMANHO_POPULACAO 50
#define TAMANHO_GENES     34
#define TAMANHO_BASE_TREINAMENTO 239
#define TAMANHO_BASE_TESTE 119
#define CLASSE_EXECUCAO 6
#define MUTACAO_QTD_REAL 10  //original era 15
#define TAXA_MUTACAO_REAL 2 //original era 3
#define TOTAL_GERACOES 50
#define EXECUCOES 10

using namespace std;

unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::minstd_rand0 generator(seed);

default_random_engine generatorDistribuicao;

/************ DEFINICAO DOS TIPOS ***************/
typedef struct {
    double peso;
    int operador;
    int valor; 
}Gene;

typedef struct {
    Gene genes[TAMANHO_GENES];
    double aptidao;  
    int classe;
}Individuo;


/************ DECLARACAO DAS FUNCOES ************/
double numeroRandomicoDouble(double inicio, double fim);
int numeroRandomicoInt(int inicio, int fim);
Individuo geraIndividuo();
void geraPopulacao(vector<Individuo>& populacao, int tamanhoP);
Individuo retornaIndividuo(vector<Individuo> populacao, int index);
void printIndividuo(Individuo individuo);
void printPopulacao(vector<Individuo> populacao);
void funcaoAvaliacaoInicial(Individuo *individuo);
//void funcaoAvaliacaoGeracao(Individuo *individuo);
void funcaoAvaliacaoFinal(Individuo *individuo);
void leBase(char *base, int tipo);
void crossover(Individuo pai1, Individuo pai2, Individuo *filho1, Individuo *filho2);
void mutacao(vector<Individuo>& populacao);
int torneioEstocastico(vector<Individuo> populacao);
int roleta(vector<Individuo> populacao, double somaAptidao);
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
//    srand(time(NULL));
    for(int i=0; i<EXECUCOES; i++)
       execucao();
    
    return 0;
}

double numeroRandomicoDouble(double inicio, double fim){
//    uniform_real_distribution<double> distribuicaoRandom(inicio, fim);
////    fim = fim*100;
//    double num = distribuicaoRandom(generator);
//    double num = double(generator()%(fim + 1 - inicio)+inicio);
    
    double num = (((double)generator()*(fim - inicio) / RAND_MAX) + inicio);
    return num;
    
}
int numeroRandomicoInt(int inicio, int fim){
//    uniform_int_distribution<int> distribuicaoInt(inicio, fim );
//    return distribuicaoInt(generator);
    return generator()%(fim+1-inicio)+inicio;
}
void printIndividuo(Individuo individuo){
    for(int i=0; i<TAMANHO_GENES; i++){
        printf("[%.2f|%d|%d]  ", individuo.genes[i].peso, individuo.genes[i].operador, individuo.genes[i].valor);
    }

    printf("ap = %.3f\n\n",individuo.aptidao);
   
//    printf("classe = %d\n\n", individuo.classe);
}
void printPopulacao(vector<Individuo> populacao){
    for(int i=0; i<populacao.size(); i++){
        printIndividuo(populacao[i]);
    }
}
void leBase(char *base, int tipo){
    
    int i=0, j=0; 
    int aux;
    
    ifstream file;
    string line;
    
    file.open(base);
    
    if(file.is_open()){
        Individuo temp;
        while(getline(file, line) ){
            istringstream iss(line);
            j=0;
            while(iss >> aux && j<35){
                if(j==34){
                    temp.classe = aux;
                    temp.aptidao = 0.0;
                    temp.genes[j].valor = 0;
                }else{
                    temp.genes[j].valor = aux;
                }
                j++;
                
            }
            if(tipo == 0){
               
                baseTreinamento.push_back(temp);
            }else if(tipo == 1){
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
        
        individuo.genes[i].peso = numeroRandomicoDouble(0.0,1.0);
        individuo.genes[i].operador = numeroRandomicoInt(0,3);
        
    }
    individuo.aptidao = 0.0;
    individuo.classe = 1000;      
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

Individuo retornaIndividuo(vector<Individuo> populacao, int index){
    return populacao[index];
}

void funcaoAvaliacaoInicial(Individuo *individuo){
    
    /*vai varrer o individuo confrontando com todos os registros da base */
    int contTP=0, contTN=0, contFP=0, contFN = 0;
    double SE, SP, aptidao;
    int VI, VB;
    bool C1, C2;

    for(int i=0; i<TAMANHO_BASE_TREINAMENTO; i++){
        C1=false;
        C2 = false;
        for(int j=0; j<TAMANHO_GENES; j++){
            //comparar gene i da base com gene j do individuo recebido
            // atribuir o incremento no devido contador
            // verificar o caso especial no gene 11 e 34 NAO FIZ ISSO 

            
            VB = baseTreinamento[i].genes[j].valor;
            VI = (*individuo).genes[j].valor;
            if(j == 11 || j == 34){ //historico ou idade
                (*individuo).genes[j].peso = 0.0;
            }
            if( (*individuo).genes[j].peso > 0.7){  

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
            }
            C2 = (baseTreinamento[i].classe == CLASSE_EXECUCAO);
        }
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
            
    if ((contTP + contFN) != 0) { 
        SE = (double)(contTP+1) / (double)(contTP + contFN + 1); //o 1 foi adicionado para ajudar na arrancada da convergencia
    }else{
         SE = (double)(contTP+1) / 1.0;
    }
    
    if((contTN+contFP) != 0){
        SP = (double)(contTN+1) / (double)(contTN + contFP + 1);
    }else{
        SP = (double)(contTN+1) / 1.0;
    }  

    (*individuo).aptidao = SE * SP;

    return;
}

void funcaoAvaliacaoFinal(Individuo *individuoFinal){
    
    leBase("testingBase.txt", 1); 
    /*vai varrer o individuo confrontando com todos os registros da base */
    int contTP=0, contTN=0, contFP=0, contFN = 0;
    double SE, SP;
    int VI, VB;
    bool C1, C2;    
    
    for(int i=0; i<TAMANHO_BASE_TESTE; i++){
        C1 = false;
        C2 = false;
        for(int j=0; j<TAMANHO_GENES; j++){
            //comparar gene i da base com gene j do individuo recebido
            // atribuir o incremento no devido contador
            // verificar o caso especial no gene 11 e 34 
//            (*individuo).genes[j].
            VB = baseTeste[i].genes[j].valor;
            VI = (*individuoFinal).genes[j].valor;
            if(j == 11 || j == 34){ //historico ou idade
                (*individuoFinal).genes[j].peso = 0.0;
            }
//            printf("peso = %f\n",  (*individuo).genes[j].peso);
            
            if( (*individuoFinal).genes[j].peso > 0.7){  
                switch((*individuoFinal).genes[j].operador){
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
            }
            C2 = (baseTeste[i].classe == CLASSE_EXECUCAO);
        }

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
    
    if ((contTP + contFN) != 0) { 
        SE = (double)(contTP+1) / (double)(contTP + contFN+1); //o 1 foi adicionado para ajudar na arrancada da convergencia
    }else{
         SE = (double)(contTP+1) / 1.0;
    }
    
    if((contTN+contFP) != 0){
        SP = (double)(contTN+1) / (double)(contTN + contFP +1);
    }else{
        SP = (double)(contTN+1) / 1.0;
    }  

    (*individuoFinal).aptidao = SE * SP;
    return;
}

void mutacao(vector<Individuo>& populacao){
    
    int aux1, aux2,aux3,i,posPeso,posOperador,posValor;
    int pes,op,op1,val,val1,ida,ida1;
    
    vector<int> populacaoMutacaoSelecionada(MUTACAO_QTD_REAL, -1);
    int temp;
    bool repetido = false;
    Individuo individuo;
    
   
    i=0;
    while(i<MUTACAO_QTD_REAL){
        temp = numeroRandomicoInt(0,MUTACAO_QTD_REAL);
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
         posPeso = numeroRandomicoDouble(0.0,1.0);
         posOperador = numeroRandomicoInt(0,3);
         posValor = numeroRandomicoInt(0,3);
         aux1 =  numeroRandomicoInt(0,9);
         aux2 =  numeroRandomicoInt(0,9);
         aux3 =  numeroRandomicoInt(0,9);
         // mutação peso
         if(aux1 < TAXA_MUTACAO_REAL)
         {
             // 1 significa que vai subtrair 0,1 e 2 significa que vai add 0,1 no peso
             pes = numeroRandomicoInt(1,2);
             if(pes == 1){
                individuo.genes[posPeso].peso = (individuo.genes[posPeso].peso<0.0)? 0.0 :(individuo.genes[posPeso].peso - 0.1);
             }else{
                individuo.genes[posPeso].peso = (individuo.genes[posPeso].peso + 0.1);
             }
         }
         //mutação simbolo
         if(aux2 < TAXA_MUTACAO_REAL)
         {
             op = numeroRandomicoInt(0,3);
             op1 = individuo.genes[posOperador].operador;
             while(op == op1)
             {
                  op = numeroRandomicoInt(0,3);
             }
             individuo.genes[posOperador].operador = op;
         }
         // mutação para valor
         if(aux3 < TAXA_MUTACAO_REAL)
         {
             val = numeroRandomicoInt(0,3);
             val1 = individuo.genes[posValor].valor;
             while(val == val1)
             {
                  val = numeroRandomicoInt(0,3);
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

            if(aux3 <= TAXA_MUTACAO_REAL)
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

int torneioEstocastico(vector<Individuo> populacao){
    
    int sorteio1, sorteio2, sorteio3, selecionado;
    double somaAptidao;
//    
    for(int i=0;i<TAMANHO_POPULACAO;i++){
        somaAptidao += populacao[i].aptidao;
    }

    sorteio1 = roleta(populacao, somaAptidao);
    sorteio2 = roleta(populacao, somaAptidao);
    sorteio3 = roleta(populacao, somaAptidao);

    selecionado = torneio(populacao, sorteio1, sorteio2, sorteio3); 
    return selecionado;
}

int roleta(vector<Individuo> populacao, double somaAptidao){
    
    double somaSelecao=0;	
    double random;
    
    uniform_real_distribution<double> distribuicaoRandom(0.0, somaAptidao);

//    binomial_distribution<double> distribuicaoRandom(0.0, somaAptidao);
    random = distribuicaoRandom(generator);
//    random = numeroRandomicoDouble(0.0, somaAptidao);
    
    int i=0;
    while(somaSelecao<random){
        i++;
        i%=TAMANHO_POPULACAO;
        somaSelecao += populacao[i].aptidao;
    }
    return i;
  
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
    
    funcaoAvaliacaoInicial(filho1);
    funcaoAvaliacaoInicial(filho2);
    return; 
}


void reinsercaoElitismo(vector<Individuo> populacaoAntiga, vector<Individuo>& populacaoNova){

    Individuo melhor = selecionaMelhor(populacaoAntiga);    
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
    int geracoes = 0;
    bool aptidaoOtima = false;
    int aptidaoOtimaIndex = -1;
    
    vector<Individuo> populacao;
    geraPopulacao(populacao, TAMANHO_POPULACAO); //gera a populacao inicial
    
    char base[] = "trainingBase.txt";             // le a base para a funcao de avaliacao
    leBase(base, 0); 
//    printPopulacao(baseTreinamento);
    
    for(int i=0; i<TAMANHO_POPULACAO; i++){
        funcaoAvaliacaoInicial(&populacao[i]);    //avalia a populacao inicial
    }
//    printPopulacao(populacao);
    //passo loop: checar se alguma aptidao == 1: se tiver retorna esse elemento e acaba execucao senao continua
    while(geracoes<TOTAL_GERACOES && !aptidaoOtima){  //rodar 50 geracoes ou ate encontrar regra com aptidao 1 
//        printf("execucao\n");
        geracoes++;
        for(int i=0; i<TAMANHO_POPULACAO; i++){
            if(populacao[i].aptidao == 1.0){
                aptidaoOtima = true;
                aptidaoOtimaIndex = i;
                break;
            }
        }
        
        if(aptidaoOtima){
            printf("Individuo otimo encontrado na posicao %d da geracao %d:\n ", aptidaoOtimaIndex, geracoes);
            printIndividuo(populacao[aptidaoOtimaIndex]);
            
            funcaoAvaliacaoFinal(&populacao[aptidaoOtimaIndex]);
            printf("BASE DE TESTE AVALIACAO\n");
            printIndividuo(populacao[aptidaoOtimaIndex]);
            double aptidaoTeste = populacao[aptidaoOtimaIndex].aptidao;
            
            printf("\nDiferenca entre treinamento e teste = %.3f\n", populacao[aptidaoOtimaIndex].aptidao - aptidaoTeste);
    
            return;
        }

        vector<Individuo> populacaoNova;

        int pai1Index, pai2Index;
        Individuo filho1, filho2, pai1, pai2;
        filho1.classe = 0;
        filho2.classe = 0;
        for(int i=0; i<TAMANHO_POPULACAO/2; i+=2){
            pai1Index = torneioEstocastico(populacao);
            pai2Index = torneioEstocastico(populacao);
            while (pai1Index == pai2Index){
                pai2Index = torneioEstocastico(populacao);
            }            
            
            pai1 = retornaIndividuo(populacao, pai1Index);
            pai2 = retornaIndividuo(populacao, pai2Index);
            
           
            crossover(pai1,pai2, &filho1, &filho2);
        
            populacaoNova.push_back(filho1);
            populacaoNova.push_back(filho2);   
        }

        //mutar os filhos 
        mutacao(populacaoNova);
      
        //selecionar o melhor e retonar a nova populacao para passo loop. 
        reinsercaoElitismo(populacao,populacaoNova);
        
        populacao = populacaoNova;
        
        aptidaoOtima = false;
        
        populacaoNova.clear();
    }
    
    double aptidaoTreinamento, aptidaoTeste;
    //geracoes acabaram, entao seleciona a melhor regra
    Individuo regraFinal = selecionaMelhor(populacao);
    printf("regra final: \n");
    printIndividuo(regraFinal);
    aptidaoTreinamento = regraFinal.aptidao;
    
    funcaoAvaliacaoFinal(&regraFinal);
    printf("BASE DE TESTE AVALIACAO\n");
    printIndividuo(regraFinal);
    aptidaoTeste = regraFinal.aptidao;
    
    printf("\nDiferenca entre treinamento e teste = %.3f\n", aptidaoTreinamento - aptidaoTeste);
//    
    baseTreinamento.clear();
    baseTeste.clear();
}