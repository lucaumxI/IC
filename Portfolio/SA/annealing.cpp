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
#include <iomanip>

using namespace std;


//-----------------------------------------Observações-------------------------------------------//
/*
    Modificacoes proposta:
        - modificar o loop que pega as entradas no SA para fazer isso automaticamente usando os valores de 0-n dos nomes dos .txt - CHECK
        - fazer funcao para salvar as informacoes pertinentes de cada dia (lambda, orcamento e custo)
        - testar TUDO novamente se está funcionando
        - verificar se é possivel otimizar algo
        - calcular o O(n)
        - ao inves de usar flag no vetorPreco criar um vetor de entrada novo p/ dizer as acoes elegiveis
        - deixar as coisas mais bonitinhas criando zilhoes de funcoes p/ por exemplo limpar e salvar os dados nos arquivos
    Condicoes para elegibilidade de uma acao:
        - a acao precisa ter um numero minimo de valores registrados na tabela. Valor minimo definido em outro algoritmo
        - para cada portfolio, uma acao pode custar no maximo uma porcentagem do orcamento. Valor porcentagem definido abaixo
        - o portfolio tem que ser menor que o orcamento. Valor orcamento definido abaixo
*/


//-----------------------------------------Parametros------------------------------------------//
float orcamento = 10000;
float alpha = 0.96;
int qntAcoes = 10;
double temperaturaInicial = 90;
float aversao = 0.01;
int cadeiaMarkov = 2*qntAcoes;
float porcentagem = 0.12;

string strCovariancia = "database/simulacao/covariancia/";
string strPreco = "database/simulacao/preco/";
string strValorEsperado = "database/simulacao/valor_esperado/";
string strFlag = "database/simulacao/flag/";
string strOrcamento = "resultados/orcamento";
string strCusto = "resultados/custo";
string strLambda = "resultados/lambda";
string strOrcamentoFinalDia = "resultados/orcamentoFinal";
string strCarteira = "resultados/vetor";
//-----------------------------------------Assinatura funções------------------------------------------//

double** startMatrizCovariancia(string caminho);
double* startVetorPreco(string caminho);
double* startVetorRetorno(string caminho);
double lambda(double* vetorPreco, double* vetorRetorno, double** covariancia, int* v);
double comparacao(double* vetorPreco, double* vetorRetorno, double** covariancia, int* v, int* possivelEstado);
double probabilidadeAceitar(double temperatura, double deltaL);
void changeSolution(int* solucao, double* preco, double* retorno, int* flag);
void startAnnealing(int* v, double* preco, double* retorno, double** covariancia, int* flag);
void copiarVetor(int* v, int* n);
float custo(double* preco, int* v);
void venderAcoes(int* v, int d);
int* startSolucao();
int porcentagemOrcamento(double* preco, float orcamento, int pos);
void annealing();
bool vetorZerado(int* v);

//----------------------------------------Funções para manipulação dos vetores de input------------------------------------------------//

double** startMatrizCovariancia(string caminho){      //Cria a matriz de covariancia
    double** matriz = new double*[qntAcoes];
    for(int i = 0; i<qntAcoes; i++)
        matriz[i] = new double[qntAcoes];

    ifstream file(caminho);    //caminho do arquivo (alterar isso pra ficar amis bonitinho)
    if (!file.is_open()) {                                                              //Veritica se o caminho ta correto
        cerr << "Erro ao abrir o arquivo covariancia" << std::strerror(errno) << endl;
        abort();
    }

    for (int i = 0; i < qntAcoes; ++i) {                                                //le os valores do arquivo
        for (int j = 0; j < qntAcoes; ++j) {
            file >> matriz[i][j];
            if (file.fail()) {
                cerr << "Erro ao ler o valor na posição [" << i << "][" << j << "]." << endl;   //se ocorrer algum erro na posicao i,j avisa
                abort();
            }
        }
    }

    file.close();

    return matriz;
}


double* startVetorPreco(string caminho){              //cria o arquivo de precos
    ifstream file(caminho, ios::in);        //abre o arquivo e verifica se conseguiu abrir
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo preco" << endl;
        abort();
    }
    
    double* valores = new double[qntAcoes];     //instancia o vetor
    
    string line;
    int index = 0;
    while (getline(file, line)) {       //vai pegando cada linha do arquivo, pois separa cada valor com \n
        if (line == "nan"){             //caso a linha seja "nan", atribui o valor como -1. nan vai ser escrito caso a acao n seja elegivel, ou seja, nao tem dados o bastante para fazer a analise, ai salva como nan para usar com flag
            valores[index] = -1;
            index++;
        }
        else{                       //se n for nan pega o valor da linha e salva no vetor
        valores[index] = stod(line);
        index++;
        }
    }
    
    file.close();
    
    if (index != qntAcoes) {
        cerr << "Erro: o arquivo preco não contém o numero especificado de elementos." << std::endl;
        abort();
    }

    return valores;
}

double* startVetorRetorno(string caminho){            //apenas pega os valores do aqrquivo e salva no vetor
    ifstream file(caminho, ios::in);
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
        cerr << "Erro: o arquivo retorno não contém o numero especificado de elementos." << std::endl;
        abort();
    }

    return valores;
}


int* startSolucao(){                    //inicia o vetor de solucao com 0 em cada posicao
    int* vetor = new int[qntAcoes];

    for (int i = 0; i<qntAcoes; i++)
        vetor[i] = 0;
    
    return vetor;
}

void copiarVetor(int* vetorOriginal, int* vetorAlvo){               //vetor alvo recebe uma copia do vetor original
    for(int i = 0; i < qntAcoes ; i++)
        vetorAlvo[i] = vetorOriginal[i];
}

int* startFlag(string caminho){
    ifstream file(caminho, ios::in);
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo flag" << endl;
        abort();
    }
    
    int* valores = new int[qntAcoes];
    
    string line;
    int index = 0;
    while (getline(file, line)) {
        valores[index] = stod(line);
        index++;
    }
    
    file.close();
    
    if (index != qntAcoes) {
        cerr << "Erro: o arquivo flag não contém o numero especificado de elementos." << std::endl;
        abort();
    }

    return valores;
}

//----------------------------------------Funções do annealing------------------------------------------------//            testar

double lambda(double* vetorPreco, double* vetorRetorno, double** covariancia, int* v){       //calculo da f.o
    double resultado = 0;
    double a = 0;
    double b = 0;
    for(int i = 0; i < qntAcoes; i++){
        for(int j = 0; j < qntAcoes; j++){
            a += covariancia[i][j]*vetorPreco[i]*vetorPreco[j]*v[i]*v[j];
        }
        b += vetorRetorno[i]*vetorPreco[i]*v[i];
    }
    resultado = aversao*a - b;
    return resultado;
}

double comparacao(double* vetorPreco, double* vetorRetorno, double** covariancia, int* v, int* possivelEstado){                  //compara se a solucao atual é melhor que a proposta
    return lambda(vetorPreco, vetorRetorno, covariancia, possivelEstado) - lambda(vetorPreco, vetorRetorno, covariancia, v);
}

double probabilidadeAceitar(double temperatura, double deltaL){  //calcula a chance de aceitar a solucao propsota mesmo sendo pior
    return (exp(-1* deltaL / temperatura));
}

float custo(double* preco, int* v){     //calculo do preco total do portfolio criado
    float soma = 0;
    for (int i = 0; i < qntAcoes; i++)
        soma += preco[i]*v[i];
    return soma;
}

void venderAcoes(int* v, int d){        //vende todas as acaos do portfolio
    if (d != 0){
        double* precoAtual = startVetorPreco(strPreco + "N" + to_string(qntAcoes) + "_" + to_string(d)  + ".txt");
        double* precoAnterior = startVetorPreco(strPreco + "N" + to_string(qntAcoes) + "_" + to_string(d)  + ".txt");
        for (int i = 0; i < qntAcoes; i++){
            if (precoAnterior[i] < precoAtual[i]){
                orcamento = orcamento + precoAtual[i]*v[i];
                v[i] = 0;
            }
        }
        if (d == 9){
            orcamento = orcamento + custo(precoAtual, v);
    }

        delete[] precoAtual;
        delete[] precoAnterior;
    }

}

//----------------------------------------Novas soluções------------------------------------------------//      testar

void changeSolution(int* solucao, double* preco, double* retorno, int* flag){       //soma 1 ou -1 em uma ação aleatoria caso ela seja elegivel
    int pos = rand() % qntAcoes;                    //sorteia a acao
    int op = 1;                            //sorteia se ira somar ou subtrair a acao

    if (flag[pos] == 1){                //compara com 0 pois definimos anteriormente que a flag seria um preco[pos] = -1
        if(op){
            solucao[pos]++;
            if(custo(preco, solucao) > orcamento)       //verifica se extrapola orcamento maximo
                solucao[pos]--;
            if(solucao[pos] > porcentagemOrcamento(preco, orcamento, pos))      //verifica se extrapola a procentagem maxima
                solucao[pos]--;
            
        }
    }
}

int porcentagemOrcamento(double* preco, float orcamento, int pos){      //retorno um int indicando quantas acoes i podemos comprar para nao extraploar o limite de ocamento p/ cada acao
    double valor = orcamento * porcentagem;

    return valor/preco[pos];
}
//----------------------------------------annealing------------------------------------------------//  
bool vetorZerado(int* v) {
    for (int i = 0; i < qntAcoes; i++) {
        if (v[i] != 0) return false;
    }
    return true;
}  
void startAnnealing(int* v, double* preco, double* retorno, double** covariancia, int* flag){
    int* possivelSolucao = startSolucao();
    copiarVetor(v , possivelSolucao);
        double temperatura = temperaturaInicial;
        
        while(temperatura > 0.0001){
            for(int i = 0; i<cadeiaMarkov; i++){
                changeSolution(possivelSolucao, preco, retorno, flag);
                double deltaL = comparacao(preco, retorno, covariancia, v, possivelSolucao);
                if(deltaL < 0)
                    copiarVetor(possivelSolucao, v);
                else{
                    double r = rand()%1000;
 
                    if((r/1000) < probabilidadeAceitar(temperatura, deltaL)){
                        copiarVetor(possivelSolucao, v);
                    }
                    else
                        copiarVetor(v , possivelSolucao);
                }
            }
            temperatura = temperatura * alpha;
        }
        orcamento = orcamento - custo(preco, v);

        delete[] possivelSolucao;
}

void annealing(){
    int dias = 10;
        int* v = startSolucao();

        /*
        ofstream file;
        file.open("resultados/custo.txt");  
        file.close();
        file.open("resultados/lambda.txt");  
        file.close();  
        file.open("resultados/orcamento.txt");  
        file.close();  
        */
        string parametro = (to_string(aversao));

        orcamento = 10000;

        for(int d = 0; d < dias; d++){   
            double* valorEsperado = startVetorRetorno(strValorEsperado + "N" + to_string(qntAcoes) + "_" + to_string(d)  + ".txt");
            double* preco = startVetorPreco(strPreco + "N" + to_string(qntAcoes) + "_" + to_string(d)  + ".txt");
            double** covariancia = startMatrizCovariancia(strCovariancia + "N" + to_string(qntAcoes) + "_" + to_string(d)  + ".txt");
            int* flag = startFlag(strFlag + "N" + to_string(qntAcoes) + "_" + to_string(d)  + ".txt");


            venderAcoes(v, d);


            ofstream arquivo;
            arquivo.open(strOrcamento + ".txt", ios::app);
            arquivo << orcamento;
            arquivo << "\n";
            arquivo.close();

            float auxOrcamento = orcamento;
            //testeParametros(preco, valorEsperado, covariancia, flag);

            orcamento = auxOrcamento;


            startAnnealing(v, preco, valorEsperado, covariancia, flag);
            /*
            cout << "Orcamento: " << orcamento << endl;

            cout << "Vetor encontrado: ";
            for (int i = 0; i < qntAcoes; i++){
                cout << v[i] << " ";
            }
            cout << endl << "Lambda: " << lambda(preco, valorEsperado, covariancia, v) << endl << "Custo: " << custo(preco, v) << endl << endl;
            */
            
            arquivo.open(strCusto + ".txt", ios::app);
            arquivo << custo(preco, v);
            arquivo << "\n";
            arquivo.close();

            arquivo.open(strLambda + ".txt", ios::app);
            arquivo << lambda(preco, valorEsperado, covariancia, v);
            arquivo << "\n";
            arquivo.close();

            arquivo.open(strCarteira + ".txt", ios::app);
            for (int i = 0; i<qntAcoes; i++)
                arquivo << v[i] << " ";
            arquivo << "\n";
            arquivo.close();

            arquivo.open(strOrcamentoFinalDia + ".txt", ios::app);
            arquivo << orcamento;
            arquivo << "\n";
            arquivo.close();

            delete preco;
            delete valorEsperado;
            delete flag;
            for (int i = 0; i < qntAcoes; ++i)
                delete[] covariancia[i];
            delete[] covariancia;   
        }
    }



#endif
