#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <time.h> 
#include <bits/stdc++.h>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <windows.h>

using namespace std;


//-----------------------------------------Parametros------------------------------------------//

float orcamento = 10000;
float alpha = 0.55;
int qntAcoes = 73;
int temperaturaInicial = 10000;
float aversao = 0.15;
int cadeiaMarkov = 20;
float porcentagem = 0.12;
int runs = 1;
bool testarParametros = false;
string caminho;

//-----------------------------------------Assinatura funções------------------------------------------//

double** startMatrizCovariancia();
double* startVetorPreco();
double* startVetorRetorno();
float lambda(double* vetorPreco, double* vetorRetorno, double** covariancia, int* v);
float comparacao(double* vetorPreco, double* vetorRetorno, double** covariancia, int* v, int* possivelEstado);
float probabilidadeAceitar(int temperatura, float deltaL);
void changeSolution(int* solucao, double* preco, double* retorno);
void startAnnealing(int* v, double* preco, double* retorno, double** covariancia);
void copiarVetor(int* v, int* n);
float custo(double* preco, int* v);
void venderAcoes(int* v, double* preco);
int* startSolucao();
int porcentagemOrcamento(double* preco, float orcamento, int pos);
void testeParametros(double* preco, double* retorno, double** covariancia);
void annealing();
void imprimeVetorSolucoes(int* v);

float mediaLambda;

//----------------------------------------Funções para manipulação dos vetores de input------------------------------------------------//

double** startMatrizCovariancia(){
    double** matriz = new double*[qntAcoes];
    for(int i = 0; i<qntAcoes; i++)
        matriz[i] = new double[qntAcoes];

    ifstream file("database\\outros ai pra eu testar\\may\\24_may/covariancia.txt");
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo covariancia" << endl;
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
    ifstream file(caminho, ios::in);
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo preco" << endl;
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
    ifstream file("database\\outros ai pra eu testar\\may\\24_may/retorno.txt", ios::in);
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo retorno" << endl;
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
    resultado = aversao*a - b;
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

void venderAcoes(int* v, double* preco){
    for (int i = 0; i < qntAcoes; i++){
        orcamento += v[i]*preco[i];
        v[i] = 0;
    }
}

//----------------------------------------Novas soluções------------------------------------------------//      testar

void changeSolution(int* solucao, double* preco, double* retorno){       //soma 1 em uma ação aleatoria
    int pos = rand() % qntAcoes;
    int op = rand() % 2;

    if(retorno[pos] > 0){
        if(op){
            solucao[pos]++;
            if(custo(preco, solucao) > orcamento)
                solucao[pos]--;
            if(solucao[pos] > porcentagemOrcamento(preco, orcamento, pos))
                solucao[pos]--;
        }
        else{
            if(solucao[pos] != 0)   
                solucao[pos]--;
        }
    }
}

int porcentagemOrcamento(double* preco, float orcamento, int pos){
    double valor = orcamento * porcentagem;

    return valor/preco[pos];
}
//----------------------------------------annealing------------------------------------------------//       testar
void testeParametros(double* preco, double* retorno, double** covariancia){
    float menorLambda = 0;
    float melhorAlpha;
    int melhorTemp;
    int melhorMarkov;
    testarParametros = true;
    mediaLambda = 0;
    int* v;
    orcamento = 10000;

    alpha = 0.85;
    temperaturaInicial = 80000;
    cadeiaMarkov = 50;
    
    while(alpha >= 0.5){
        for(int i = 0; i<50; i++){
            v = startSolucao();
            startAnnealing(v, preco, retorno, covariancia);
            orcamento = 10000;
        }
        mediaLambda = mediaLambda/50;
        if (menorLambda == 0){
            menorLambda = mediaLambda;
            melhorAlpha = alpha;
        }
        if (menorLambda > mediaLambda){
            menorLambda = mediaLambda;
            melhorAlpha = alpha;
        }
        alpha = alpha - 0.05;
        mediaLambda = 0; 
    }
    alpha = 0.9;
    menorLambda = 0;
    cout << "Terminou alpha" << endl;

    
    temperaturaInicial = 300000;
    while(temperaturaInicial >= 50000){
        for(int i = 0; i<50; i++){
            int* v = startSolucao();
            startAnnealing(v, preco, retorno, covariancia);
            orcamento = 10000;
        }
        mediaLambda = mediaLambda/50;
        if (menorLambda == 0)
            menorLambda = mediaLambda;
        if (menorLambda > mediaLambda){
            menorLambda = mediaLambda;
            melhorTemp = temperaturaInicial;
        }
        temperaturaInicial = temperaturaInicial - 10000;
        mediaLambda = 0;
    }
    temperaturaInicial = 80000;
    menorLambda = 0;
    cout << "Terminou temperatura" << endl;

    cadeiaMarkov = 200;
    while(cadeiaMarkov >= 1){
        for(int i = 0; i<50; i++){
            int* v = startSolucao();
            startAnnealing(v, preco, retorno, covariancia);
            orcamento = 10000;
        }
        mediaLambda = mediaLambda/50;
        if (menorLambda == 0)
            menorLambda = mediaLambda;
        if (menorLambda > mediaLambda){
            menorLambda = mediaLambda;
            melhorMarkov = cadeiaMarkov;
        }
        cadeiaMarkov = cadeiaMarkov - 20;
    }
    cout << "Terminou Markov" << endl;

    alpha = melhorAlpha;
    cadeiaMarkov = melhorMarkov;
    temperaturaInicial = melhorTemp;
}

void startAnnealing(int* v, double* preco, double* retorno, double** covariancia){ 
    int* possivelSolucao = startSolucao();    
        for (int p = 0; p < runs; p++){
            int temperatura = temperaturaInicial;
            while(temperatura > 0.5){
                for(int i = 0; i<cadeiaMarkov; i++){
                    changeSolution(possivelSolucao, preco, retorno);
                    float deltaL = comparacao(preco, retorno, covariancia, v, possivelSolucao);
                    if(deltaL < 0)
                        copiarVetor(possivelSolucao, v);
                    else{
                        if((rand()%1000) < probabilidadeAceitar(temperatura, deltaL) * 1000)
                            copiarVetor(possivelSolucao, v);
                    }
                }
                temperatura = temperatura * alpha;
            }
        }

        orcamento = orcamento - custo(preco, v);
        
        if (testarParametros){
            mediaLambda += lambda(preco, retorno, covariancia, v);
        }
        delete possivelSolucao;
}

void annealing(){  
    double* retorno = startVetorRetorno();
    double* preco;
    double** covariancia = startMatrizCovariancia();
    int* v = startSolucao();
    bool flag = true;

    while(flag){
        cout << "Digite o caminho do arquivo de precos: ";
        getline(cin, caminho);
        preco = startVetorPreco();

        venderAcoes(v, preco);
        float auxOrcamento = orcamento;
        testeParametros(preco, retorno, covariancia);

        orcamento = auxOrcamento;
        cout << "Orcamento: " << orcamento << endl;
        startAnnealing(v, preco, retorno, covariancia);

        cout << "Vetor encontrado: ";
        for (int i = 0; i < qntAcoes; i++){
            cout << v[i] << " ";
        }
        cout << endl << "Lambda: " << lambda(preco, retorno, covariancia, v) << endl << "Custo: " << custo(preco, v) << endl << endl;

        delete preco;   

        Beep(750, 1000);
        int escolha;
        cout << "1 - Ir para o próximo dia" << endl << "2 - Encerrar" << endl;
        cin >> escolha;
        cin.ignore();

        if (escolha == 2)   //implementar aqui para vender os ações de acordo com o valor do proximo dia e salvar num arquivo txt
            flag = false;   
    }
    delete retorno;
    for (int i = 0; i < qntAcoes; ++i)
        delete[] covariancia[i];
    delete[] covariancia;
}




#endif