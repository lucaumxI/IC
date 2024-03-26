#include <iostream>
#include <time.h> 
#include <bits/stdc++.h>
#include <fstream>
#include <sstream>

//  MEXER NAS FUNCOES COMPARACAO E PROBABILIDADEDEACEITAR, ELAS ESTAO USANDO A LOGICA DE SÓ TER O SWAP MAS ADICIONEI MAIS DOIS MÉTODOS


using namespace std;

int tamanho = 48;
float temperatura = 4000;
float alpha = 0.99;
int repeticoes = 60000;
int minimo = 33523;

int** newMatriz(int tamanho){
    int** matriz = new int*[tamanho];
    for (int i = 0; i<tamanho; i++){
        matriz[i] = new int[tamanho];
    }

    return matriz;
}

void preencheMatriz(int tamanho, int** matriz){
    string nomeArquivo = "database/48 cidades (33523)/distance.txt";
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

void swap(int* v, int posA, int posB){
    int aux = v[posA];
    v[posA] = v[posB];
    v[posB] = aux;
}

void inverse(int* v, int posA, int posB){
    int aux;
    if(posA<posB){
        for(posA; posA<posB; posA++){
            aux = v[posA];
            v[posA] = v[posB];
            v[posB] = aux;
            posB--;
        }
    }
    else{
        for(posB; posB<posA; posB++){
            aux = v[posA];
            v[posA] = v[posB];
            v[posB] = aux;
            posA--;
        }
    }
}

void insert(int* v, int posA, int posB){
    int aux;

    aux = v[posA];

    if(posA < posB){
        for(int i = posA; i<posB; i++){
            v[i] = v[i + 1];
        }
        v[posB] = aux;
    }
    else{
        for(int i = posA; i>posB; i--){
            v[i] = v[i - 1];
        }
        v[posB] = aux;
    }


}

void changeSolution(int* v, int posA, int posB){
    int func = rand() % 3;

    switch(func){
        case 0:
            inverse(v, posA, posB);
            break;
        case 1:
            swap(v, posA, posB);
            break;
        case 2:
            insert(v, posA, posB);
            break;
    }

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

int* startAnneling(int v[], int** matrizCusto){
    int* rotaMin = new int[tamanho + 1];
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
            changeSolution(v, posA, posB);
            mesmaSolucao = 0;
        }
        else{
            if ((rand() % 1000) < probabilidadeAceitar(v, posA, posB, temperatura, matrizCusto, deltaC) * 1000){
                changeSolution(v, posA, posB); 
                mesmaSolucao = 0;
            }
            else
                mesmaSolucao += 1;
            
        }
        
        
        if (custoMin > custo(v, matrizCusto)){
            for (int i = 0; i<tamanho + 1; i++){
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
    for (int i = 0; i<tamanho + 1; i++)
        cout << rotaMin[i] + 1 << " ";
    cout << endl << "Custo minimo: " << custoMin << endl;

    return rotaMin;
}


int main(){
    srand(time(NULL));

    int** matrizCusto = newMatriz(tamanho);
    preencheMatriz(tamanho, matrizCusto);

    int* v = new int[tamanho + 1];
    v[0] = 0;
    v[tamanho] = 0;
    for (int i = 1; i<tamanho; i++){
        v[i] = i;
    }

    v = startAnneling(v, matrizCusto);

    for (int i = 0; i < tamanho; ++i)
        delete[] matrizCusto[i];
    delete[] matrizCusto;

    return 0;
}
