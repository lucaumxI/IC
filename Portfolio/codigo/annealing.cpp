#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <time.h> 
#include <bits/stdc++.h>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;


//-----------------------------------------Parametros------------------------------------------//

float orcamento = 5000;
float alpha = 0.9;
int qntAcoes = 99;
int temperaturaInicial = 5000;
float aversao = 0.5;
int cadeiaMarkov = 1;
float porcentagem = 0.15;
int runs = 1;

//-----------------------------------------Assinatura funções------------------------------------------//

double** startMatrizCovariancia();
double* startVetorPreco();
double* startVetorRetorno();
float lambda(double* vetorPreco, double* vetorRetorno, double** covariancia, int* v);
float comparacao(double* vetorPreco, double* vetorRetorno, double** covariancia, int* v, int* possivelEstado);
float probabilidadeAceitar(int temperatura, float deltaL);
void changeSolution(int* solucao, double* preco);      //||
void startAnnealing(int* v);
void copiarVetor(int* v, int* n);
float custo(double* preco, int* v);
int* startSolucao();                    //||
void bitFlip(int* solucao, double* preco);
int porcentagemOrcamento(double* preco, float orcamento, int pos);
void addAcao(int* solucao, double* preco);

//----------------------------------------Funções para manipulação dos vetores de input------------------------------------------------//

double** startMatrizCovariancia(){
    double** matriz = new double*[qntAcoes];
    for(int i = 0; i<qntAcoes; i++)
        matriz[i] = new double[qntAcoes];

    ifstream file("database/99_ativos/matriz_covariancia_10.txt");
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo " << endl;
        abort();
    }

    for (int i = 0; i < qntAcoes; ++i) {
        for (int j = 0; j < qntAcoes; ++j) {
            file >> matriz[i][j];
            if (file.fail()) {
                cerr << "Erro ao ler o valor na posição [" << i << "][" << j << "]." << endl;
                abort();
            }
        }
    }

    file.close();

    return matriz;
}


double* startVetorPreco(){
    ifstream file("database/99_ativos/precos_10.txt", ios::in);
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo " << endl;
        abort();
    }
    
    double* valores = new double[qntAcoes];
    
    string line;
    int index = 0;
    while (getline(file, line)) {
        valores[index] = stod(line);
        index++;
    }
    
    file.close();
    
    if (index != qntAcoes) {
        cerr << "Erro: o arquivo não contém exatamente 99 linhas." << std::endl;
        abort();
    }

    return valores;
}

double* startVetorRetorno(){
    ifstream file("database/99_ativos/retornos_esperados_10.txt", ios::in);
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo " << endl;
        abort();
    }
    
    double* valores = new double[qntAcoes];
    
    string line;
    int index = 0;
    while (getline(file, line)) {
        valores[index] = stod(line);
        index++;
    }
    
    file.close();
    
    if (index != qntAcoes) {
        cerr << "Erro: o arquivo não contém exatamente 99 linhas." << std::endl;
        abort();
    }

    return valores;
}


int* startSolucao(){
    int* vetor = new int[qntAcoes];

    for (int i = 0; i<qntAcoes; i++)
        vetor[i] = 0;
    
    return vetor;
}

void copiarVetor(int* vetorOriginal, int* vetorAlvo){
    for(int i = 0; i < qntAcoes ; i++)
        vetorAlvo[i] = vetorOriginal[i];
}

//----------------------------------------Funções do annealing------------------------------------------------//            testar

float lambda(double* vetorPreco, double* vetorRetorno, double** covariancia, int* v){
    float resultado = 0;
    float a = 0;
    float b = 0;
    for(int i = 0; i < qntAcoes; i++){
        for(int j = 0; j < qntAcoes; j++){
            a += covariancia[i][j]*vetorPreco[i]*vetorPreco[j]*v[i]*v[j];
        }
        b += vetorRetorno[i]*vetorPreco[i]*v[i];
    }
    resultado = -1*(aversao*a - b);
    return resultado;
}

float comparacao(double* vetorPreco, double* vetorRetorno, double** covariancia, int* v, int* possivelEstado){
    return lambda(vetorPreco, vetorRetorno, covariancia, possivelEstado) - lambda(vetorPreco, vetorRetorno, covariancia, v);
}

float probabilidadeAceitar(int temperatura, float deltaL){
    return (exp(-1* deltaL / temperatura));
}

float custo(double* preco, int* v){
    float soma = 0;
    for (int i = 0; i < qntAcoes; i++)
        soma += preco[i]*v[i];
    return soma;
}

//----------------------------------------Novas soluções------------------------------------------------//      testar

void changeSolution(int* solucao, double* preco){       //soma 1 em uma ação aleatoria
    int function = rand() % 2;

    switch (function){
        case 0:
            bitFlip(solucao, preco);
            break;
        case 1:
            addAcao(solucao, preco);
    }
}

void bitFlip(int* solucao, double* preco){
    int pos = rand() % 99;

    if(solucao[pos] > 0)
        solucao[pos] = 0;
    else{
        solucao[pos] = rand() % porcentagemOrcamento(preco, orcamento, pos);
        while(custo(preco, solucao) > orcamento){
            solucao[pos]--;
        }
    }
}

void addAcao(int* solucao, double* preco){
    int pos = rand() % 99;

    if(solucao[pos] < porcentagemOrcamento(preco, orcamento, pos)){
        solucao[pos]++;
        if(custo(preco, solucao) > orcamento)
            solucao[pos]--;
    }
}

int porcentagemOrcamento(double* preco, float orcamento, int pos){
    double valor = orcamento * porcentagem;

    return valor/preco[pos];
}

//----------------------------------------annealing------------------------------------------------//       testar

void startAnnealing(int* v){
    double* preco = startVetorPreco();
    double* retorno = startVetorRetorno();
    double** covariancia = startMatrizCovariancia();

    int retornoMax[qntAcoes];
    copiarVetor(v, retornoMax);
    int lambdaMax = lambda(preco, retorno, covariancia, v);

    int temperatura = temperaturaInicial;

    int possivelSolucao[qntAcoes];
    copiarVetor(v, possivelSolucao);

    while(temperatura > 0.5){
        for(int i = 0; i<cadeiaMarkov; i++){
            changeSolution(possivelSolucao, preco);

            bool flag = false;
            while(!flag){
                if(custo(preco, possivelSolucao) > orcamento)
                    changeSolution(possivelSolucao, preco);
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
    cout << "Lambda: " << lambda(preco, retorno, covariancia, v) << endl << "Custo: " << custo(preco, v) << endl;
}

#endif
