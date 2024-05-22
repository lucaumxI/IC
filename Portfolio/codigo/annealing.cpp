#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <time.h> 
#include <bits/stdc++.h>
#include <fstream>
#include <sstream>

using namespace std;


//-----------------------------------------Parametros------------------------------------------//

float orcamento = 100;
float alpha = 0.9;
int qntAcoes = 50;
int temperaturaInicial = 5000;
float aversao = 0;
int cadeiaMarkov = 1;

//-----------------------------------------Assinatura funções------------------------------------------//

int** startMatrizCovariancia();
int* startVetorPreco();
int* startVetorRetorno();
float lambda(int* vetorPreco, int* vetorRetorno, int** covariancia, int* v);
float comparacao(int* vetorPreco, int* vetorRetorno, int** covariancia, int* v, int* possivelEstado);
float probabilidadeAceitar(int temperatura, float deltaL);
void changeSolution(int* solucao);
void startAnnealing(int* v);
void copiarVetor(int* v, int* n);
float custo(int* preco, int* v);
int* startSolucao();

//----------------------------------------Funções para manipulação dos vetores de input------------------------------------------------//

int** startMatrizCovariancia(){     // NÃO IMPLEMENTEI NENHUMA POIS AINDA VOU VER COMO SE DÁ A ENTRADA

}

int* startVetorPreco(){

}

int* startVetorRetorno(){

}

int* startSolucao(){

}

void copiarVetor(int* vetorOriginal, int* vetorAlvo){
    for(int i = 0; i < qntAcoes ; i++)
        vetorAlvo[i] = vetorOriginal[i];
}

//----------------------------------------Funções do annealing------------------------------------------------//

float lambda(int* vetorPreco, int* vetorRetorno, int** covariancia, int* v){
    float resultado = 0;
    float a = 0;
    float b = 0;
    for(int i = 0; i < qntAcoes; i++){
        for(int j = 0; j < qntAcoes; j++){
            a += covariancia[i][j]*vetorPreco[i]*vetorPreco[j]*v[i]*v[j];
        }
        b += vetorRetorno[i]*vetorPreco[i]*v[i];
    }
    resultado = -1*aversao*a - b;
    return resultado;
}

float comparacao(int* vetorPreco, int* vetorRetorno, int** covariancia, int* v, int* possivelEstado){
    return lambda(vetorPreco, vetorRetorno, covariancia, possivelEstado) - lambda(vetorPreco, vetorRetorno, covariancia, v);
}

float probabilidadeAceitar(int temperatura, float deltaL){
    return (exp(-1* deltaL / temperatura));
}

float custo(int* preco, int* v){
    float soma = 0;
    for (int i = 0; i < qntAcoes; i++)
        soma += preco[i]*v[i];
    return soma;
}

//----------------------------------------Novas soluções------------------------------------------------//

void changeSolution(int* solucao){  //PRECISO PENSAR AINDA EM COMO CRIAR NOVAS SOLUÇÕES PARA ESSE PROBLEMA

}

//----------------------------------------annealing------------------------------------------------//

void startAnnealing(int* v){
    int* preco = startVetorPreco();
    int* retorno = startVetorRetorno();
    int** covariancia = startMatrizCovariancia();

    int retornoMax[qntAcoes];
    copiarVetor(v, retornoMax);
    int lambdaMax = lambda(preco, retorno, covariancia, v);

    int temperatura = temperaturaInicial;

    int possivelSolucao[qntAcoes];
    copiarVetor(v, possivelSolucao);

    while(temperatura > 0.5){
        for(int i = 0; i<cadeiaMarkov; i++){
            changeSolution(possivelSolucao);

            bool flag = false;
            while(!flag){
                if(custo(preco, possivelSolucao) > orcamento)
                    changeSolution(possivelSolucao);
                else    
                    flag = true;
            }

            float deltaL = comparacao(preco, retorno, covariancia, v, possivelSolucao);
            if(deltaL < 0)
                copiarVetor(possivelSolucao, v);
            else{
                if((rand()%1000) < probabilidadeAceitar(temperatura, deltaL) * 1000);
                    copiarVetor(possivelSolucao, v);
            }

            if(lambdaMax < lambda(preco, retorno, covariancia, v)){
                copiarVetor(v, retornoMax);
                lambdaMax = lambda(preco, retorno, covariancia, v);
            }
        }
        temperatura = temperatura * alpha;
    }
}

#endif