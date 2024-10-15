#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <fstream>
#include <cmath>
#include <time.h>
#include <iomanip>
#include <windows.h>

using namespace std;


//----------------------------------------------------------------------//
int tamPopulation = 10;
int qntAcoes = 99;
float aversao = 0.15;
int qntdFilhos = 10;
int generations = 10;
float A = 0.1;
float orcamento = 10000;
//----------------------------------------------------------------------//
float** startPopulation();
float** randomPopulation();
float** warmPopulation();
//----------------------------------------------------------------------//
void fitnessCalculation(float** population, double* vetorPreco, double* vetorRetorno, double** covariancia, int numCromossome);
float fitness(float** population, int i, double* vetorPreco, double* vetorRetorno, double** covariancia);
float maxB(float* cromossome, double* vetorPreco, float B, float A);
float custo(float* cromossome, double* vetorPreco);
//----------------------------------------------------------------------//
int* selection(float** population);
int rouletteWheel(float** population);
float encontrarMenorValor(float** population);
float somaFitnessAjustado(float** population, float ajuste);
//----------------------------------------------------------------------//
float** mating(float** population, int* parents);
float** crossover(float** population, int* parents, int i);
void mutation();
//----------------------------------------------------------------------//
void offspring(float** population, float** childrens, double* vetorPreco, double* vetorRetorno, double** covariancia, int numCromossomes);
float** addOffspring(float** population, float** childrens);
void sortPopulation(float** newPopulation, double* vetorPreco, double* vetorRetorno, double** covariancia, int numCromossomes);
void merge(float** arr, int left, int mid, int right, int M);
void mergeSort(float** arr, int left, int right, int M);
void deletePopulation(float** newPopulation, float** population);
//----------------------------------------------------------------------//
double** startMatrizCovariancia();
double* startVetorPreco();
double* startVetorRetorno();
//----------------------------------------------------------------------//
void startGA();
//----------------------------------------------------------------------//


float** startPopulation(){
    int choice;
    cout << "Random - 1" << endl << "Warm - 2" << endl;
    cin >> choice;

    if(choice == 1)
        return randomPopulation();
    else    
       return warmPopulation();
}

float** randomPopulation(){
    float** population = new float*[tamPopulation];
    for(int i = 0; i<tamPopulation; i++)
        population[i] = new float[qntAcoes + 1];

    return population;
}

float** warmPopulation(){
    float** population = new float*[tamPopulation];
    for(int i = 0; i<tamPopulation; i++)
        population[i] = new float[qntAcoes + 1];
    
    ifstream file("database/99_ativos/warmvector.txt");
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo " << endl;
        abort();
    }

    for (int i = 0; i < tamPopulation; ++i) {
        for (int j = 0; j < qntAcoes; ++j) {
            file >> population[i][j];
            if (file.fail()) {
                cerr << "Erro ao ler o valor na posição [" << i << "][" << j << "]." << endl;
                abort();
            }
        }
    }

    file.close();

    return population;
}

//----------------------------------------------------------------------// tirei um abort caso numero positivo
void fitnessCalculation(float** population, double* vetorPreco, double* vetorRetorno, double** covariancia, int numCromossomes){
    for(int i = 0; i<numCromossomes; i++){
        float lambda = fitness(population, i, vetorPreco, vetorRetorno, covariancia);
        population[i][qntAcoes] = lambda;
    }
}

float fitness(float** population, int cromossome, double* vetorPreco, double* vetorRetorno, double** covariancia){
    float resultado = 0;
    float a = 0;
    float b = 0;
    for(int i = 0; i < qntAcoes; i++){
        for(int j = 0; j < qntAcoes; j++){
            a += covariancia[i][j]*vetorPreco[i]*vetorPreco[j]*population[cromossome][i]*population[cromossome][j];
        }
        b += vetorRetorno[i]*vetorPreco[i]*population[cromossome][i];
    }
    float maxi = maxB(population[cromossome], vetorPreco, orcamento, A);
    resultado = b - aversao*a - maxi;

    return resultado;
}

float maxB(float* cromossome, double* vetorPreco, float orcamento, float A){
    float m;
    if (custo(cromossome, vetorPreco) > orcamento)
        m = A * pow(custo(cromossome, vetorPreco) - orcamento, 2);

    return m;
}

float custo(float* cromossome, double* vetorPreco){
    float a = 0;
    for (int i = 0; i<qntAcoes; i++)
        a += cromossome[i]*vetorPreco[i];
    
    return a;
}
//----------------------------------------------------------------------//
int* selection(float** population){
    int *parents = new int[qntdFilhos];

    for (int i = 0; i<qntdFilhos; i++){
        parents[i] = rouletteWheel(population);
    }

    return parents;
}

int rouletteWheel(float** population){
    float menorValor = encontrarMenorValor(population);
    float ajuste = -1*menorValor;

    float somaFitness = somaFitnessAjustado(population, ajuste);
	float valorSorteio = (static_cast<float>(rand()) / RAND_MAX) * somaFitness;

    float soma = 0;
    for (int i = 0; i < tamPopulation; ++i) {
        soma += population[i][qntAcoes] + ajuste;
        if (soma >= valorSorteio) {
            return i;
        }
    }

    return -1;
}

float encontrarMenorValor(float** population){
    float menor = population[0][qntAcoes];
    for(int i = 1; i< tamPopulation; i++){
        if (menor > population[i][qntAcoes]){
            menor = population[i][qntAcoes];
        }
    }

    return menor;
}

float somaFitnessAjustado(float** population, float ajuste){
    float soma = 0.0;
    for (int i = 0; i < tamPopulation; ++i) {
        soma += population[i][qntAcoes] + ajuste;
    }
    return soma;
}
//----------------------------------------------------------------------//
float** mating(float** population, int* parents){
    float** childrens = new float*[qntdFilhos];

    for (int i = 0; i<qntdFilhos; i = i+2){
        float** prole = new float*[2];
        prole = crossover(population, parents, i);

        childrens[i] = prole[0];
        childrens[i + 1] = prole[1];

        delete prole;
    }
    

    return childrens;
}

float** crossover(float** population, int* parents, int i){
    float* childrenA = new float[qntAcoes + 1];
    float* childrenB = new float[qntAcoes + 1];

    int point = rand() % qntAcoes;

    for (int j = 0; j < point; j++){
        childrenA[j] = population[parents[i]][j];
        childrenB[j] = population[parents[i + 1]][j];
    }

    for (int j = point; j < qntAcoes; j++){
        childrenA[j] = population[parents[i + 1]][j];
        childrenB[j] = population[parents[i]][j];
    }

    float** prole = new float*[2];

    prole[0] = childrenA;
    prole[1] = childrenB;

    return prole;
}

void mutation(){
    cout << "a";
}
//----------------------------------------------------------------------//
void offspring(float** population, float** childrens, double* vetorPreco, double* vetorRetorno, double** covariancia, int numCromossomes){
    float** newPopulation = new float*[tamPopulation + qntdFilhos];
    newPopulation = addOffspring(population, childrens);;
    sortPopulation(newPopulation, vetorPreco, vetorRetorno, covariancia, numCromossomes);
    deletePopulation(newPopulation, population);
}

float** addOffspring(float** population, float** childrens){
    float** newPopulation = new float*[tamPopulation + qntdFilhos];

    for(int i = 0; i<tamPopulation; i++)
        newPopulation[i] = population[i];


    for(int i = 0; i<qntdFilhos; i++)
        newPopulation[i + tamPopulation] = childrens[i];

    return newPopulation;
}

void sortPopulation(float** newPopulation, double* vetorPreco, double* vetorRetorno, double** covariancia, int numCromossomes){
    fitnessCalculation(newPopulation, vetorPreco, vetorRetorno, covariancia, numCromossomes);
    mergeSort(newPopulation, 0, tamPopulation + qntdFilhos -1, qntAcoes);
}

void mergeSort(float** matrix, int left, int right, int M) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        mergeSort(matrix, left, mid, M);
        mergeSort(matrix, mid + 1, right, M);

        merge(matrix, left, mid, right, M);
    }
}

void merge(float** matrix, int left, int mid, int right, int M) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Vetores temporários para armazenar os sub-arranjos
    float** L = new float*[n1];
    float** R = new float*[n2];

    for (int i = 0; i < n1; ++i) {
        L[i] = new float[M + 1];
        for (int j = 0; j <= M; ++j) {
            L[i][j] = matrix[left + i][j];
        }
    }
    for (int i = 0; i < n2; ++i) {
        R[i] = new float[M + 1];
        for (int j = 0; j <= M; ++j) {
            R[i][j] = matrix[mid + 1 + i][j];
        }
    }

    int i = 0, j = 0;
    int k = left;

    // Mescla os sub-arranjos de volta em matrix[left..right]
    while (i < n1 && j < n2) {
        if (L[i][M] >= R[j][M]) {
            for (int x = 0; x <= M; ++x) {
                matrix[k][x] = L[i][x];
            }
            i++;
        } else {
            for (int x = 0; x <= M; ++x) {
                matrix[k][x] = R[j][x];
            }
            j++;
        }
        k++;
    }

    // Copia os elementos restantes de L[], se houver
    while (i < n1) {
        for (int x = 0; x <= M; ++x) {
            matrix[k][x] = L[i][x];
        }
        i++;
        k++;
    }

    // Copia os elementos restantes de R[], se houver
    while (j < n2) {
        for (int x = 0; x <= M; ++x) {
            matrix[k][x] = R[j][x];
        }
        j++;
        k++;
    }

    // Libera a memória alocada
    for (int i = 0; i < n1; ++i) {
        delete[] L[i];
    }
    for (int i = 0; i < n2; ++i) {
        delete[] R[i];
    }
    delete[] L;
    delete[] R;
}


void deletePopulation(float** newPopulation, float** population){
    for (int i = 0; i<tamPopulation; i++)
        population[i] = newPopulation[i];

    for (int i = 0; i<qntdFilhos + tamPopulation; i++)
        delete []newPopulation[i];
    delete []newPopulation;
}

//----------------------------------------------------------------------//

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

//----------------------------------------------------------------------//

void startGA(){
    float** population = startPopulation();
    double* preco = startVetorPreco();
    double* retorno = startVetorRetorno();
    double** covariancia = startMatrizCovariancia();
    int skip;
    for (int i = 0; i<generations; i++){
        fitnessCalculation(population, preco, retorno, covariancia, tamPopulation);

        int* parents = selection(population);

        float** childrens = mating(population, parents);
        offspring(population, childrens, preco, retorno, covariancia, tamPopulation + qntdFilhos);

        for (int i = 0; i<tamPopulation; i++){
            cout << i << ": ";
            for(int j = 0; j<qntAcoes; j++){
                cout << population[i][j] << " ";
            }
            cout << "Lambda: " << population[i][qntAcoes] << endl;
            
        }
        cout <<  "Aperte enter para ir para a proxima geracao.";
        Beep(750, 2000);
        cin >> skip;
    }
    delete preco;
    delete retorno;
    for (int i = 0; i<tamPopulation; i++){
        delete []population[i];
        delete []covariancia[i];
    }
    delete []population;
    delete []covariancia;
}
#endif
