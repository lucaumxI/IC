#include <iostream>
#include <time.h> 
#include <bits/stdc++.h>
#include <fstream>
#include <sstream>

using namespace std;

int tamanho = 17;
float temperatura = 5000;
float alpha = 0.99;
int repeticoes = 50000;
int minimo = 2085;

int** newMatriz(int tamanho){
    int** matriz = new int*[tamanho];
    for (int i = 0; i<tamanho; i++){
        matriz[i] = new int[tamanho];
    }

    return matriz;
}

void preencheMatriz(int tamanho, int** matriz){
    string nomeArquivo = "database/17 cidades (2085)/distance.txt";
    ifstream arquivo(nomeArquivo);

    string linha;
    int linhas = 0;
    int colunas = 0;

    // Conta o número de linhas e colunas
    while (getline(arquivo, linha)) {
        ++linhas;
        istringstream iss(linha);
        int valor;
        while (iss >> valor) {
            ++colunas;
        }
    }

    // Reposiciona o cursor do arquivo para o início
    arquivo.clear();
    arquivo.seekg(0);

    for (int i = 0; i < linhas; ++i) {
        getline(arquivo, linha);
        istringstream iss(linha);
        for (int j = 0; j < colunas / linhas; ++j) {
            int valor;
            iss >> valor;
            matriz[i][j] = valor;
        }
    }
}

float custo(int v[], int** matrizCusto){
    float soma = 0;
    for (int cidadeAtual = 0; cidadeAtual < tamanho ; cidadeAtual++){
        soma += matrizCusto[v[cidadeAtual]][v[cidadeAtual + 1]];
    }

    return soma;
}

void swap(int v[], int posA, int posB){
    int aux = v[posA];
    v[posA] = v[posB];
    v[posB] = aux;
}

float comparacao(int v[], int** matrizCusto, int posA, int posB){

    int possivelEstado[tamanho + 1];

    for (int i = 0; i<tamanho + 1 ;i++){
        possivelEstado[i] = v[i];
    }

    swap(possivelEstado, posA, posB);

    return custo(possivelEstado, matrizCusto) - custo(v, matrizCusto);
}

float probabilidadeAceitar(int v[], int posA, int posB, float temp, int** matrizCusto, float deltaC){
    int possivelEstado[tamanho + 1];

    for (int i = 0; i<tamanho + 1 ;i++){
        possivelEstado[i] = v[i];
    }

    swap(possivelEstado, posA, posB);

    return (exp(-1* deltaC / temp));
}

void startAnneling(int v[], int** matrizCusto){
    int rotaMin[tamanho + 1];
    for (int i = 0; i<tamanho; i++)
        rotaMin[i] = v[i];
    int custoMin = custo(v, matrizCusto);
    int cont = 0;
    int mesmaSolucao = 0;

    while (mesmaSolucao < repeticoes){
        int posA = 0, posB = 0;
        while (posA == posB){
            posA = rand() % (tamanho - 1) + 1;
            posB = rand() % (tamanho - 1) + 1;
        }
        float deltaC = comparacao (v, matrizCusto, posA, posB);
        if (deltaC < 0){
            swap(v, posA, posB);
            mesmaSolucao = 0;
        }
        else{
            if ((rand() % 1000) < probabilidadeAceitar(v, posA, posB, temperatura, matrizCusto, deltaC) * 1000){
                swap(v, posA, posB); 
                mesmaSolucao = 0;
            }
            else
                mesmaSolucao += 1;
            
        }
        
        
        if (custoMin > custo(v, matrizCusto)){
            for (int i = 0; i<tamanho; i++){
                rotaMin[i] = v[i];
                custoMin = custo(v, matrizCusto);
            }
        }
        temperatura = temperatura * alpha;
        cont++;
    }

    cout << "numero solucao errada = " << cont - repeticoes << endl;
    cout << "Diferenca minimo achado pro real = " << custoMin - minimo << endl;
    cout<< "Rota minima: ";
    for (int i = 0; i<tamanho; i++)
        cout << rotaMin[i] + 1 << " ";
    cout << endl << "Custo minimo: " << custoMin << endl;
}


int main(){
    srand(time(NULL));

    int** matrizCusto = newMatriz(tamanho);
    preencheMatriz(tamanho, matrizCusto);

    int v[tamanho + 1];
    v[0] = 0;
    v[tamanho] = 0;
    for (int i = 1; i<tamanho; i++){
        v[i] = i;
    }
    
    int cont = 0;
    startAnneling(v, matrizCusto);

    for (int i = 0; i < tamanho; ++i)
        delete[] matrizCusto[i];
    delete[] matrizCusto;

    return 0;
}
