#ifndef NODE_H
#define NODE_H
#include <iostream>
#include <time.h> 
#include <bits/stdc++.h>
#include <fstream>
#include <sstream>

using namespace std;

typedef struct Node *NodePtr;


//-----------------------------------------Parametros------------------------------------------//
int tamanho = 10;
float temperaturaInicial = tamanho * 100;
float alpha = 0.99;
int loopInterno = 2*tamanho;
int minimo = 33523;
int runs = tamanho*tamanho;

//------------------------------------Declaração Lista Encadeada-----------------------------------------------//
struct Node{
    int info;
    struct Node *next;
};
typedef struct Node *nodePt;

typedef struct{
    nodePt primeiro;
    nodePt ultimo;
}listaEncadeada;
//----------------------------------------Assinatura das funções-------------------------------------------//

void criar(listaEncadeada *l);
bool vazio(listaEncadeada *l);
void startLista(listaEncadeada *l);
void insert(listaEncadeada *l, int elemento);
void swap(listaEncadeada *l, int posA, int posB);
void reinsert(listaEncadeada *l, int posA, int posB);
void inverse(listaEncadeada *l, int posA, int posB);
void reinsertSubRote(listaEncadeada *l, int posA, int posB, int posC);
void imprime(listaEncadeada *l);
float custo(listaEncadeada *l, int** matrizCusto);
int** newMatriz(int tamanho);
void preencheMatriz(int tamanho, int** matriz);

//----------------------------------------------Funções para manipulação da lista------------------------------------------//

void criar(listaEncadeada *l){
    l->primeiro = NULL;
    l->ultimo = NULL;
}

bool vazio(listaEncadeada *l){
    bool flag;
    if(l->primeiro == NULL)
        flag = true;
    else  
        flag = false;

    return flag;
}

void startLista(listaEncadeada *l){
    int cont = 0;
    
    while (cont != tamanho){
        insert(l, cont);
        cont++;
    }
}

void insert(listaEncadeada *l, int elemento){
    nodePt lAux = new struct Node;
    lAux->info = elemento;
    if(vazio(l)){
        l->primeiro = lAux;
        l->ultimo = lAux;
        l->ultimo->next = NULL;
    }
    else{
        l->ultimo->next = lAux;
        l->ultimo = lAux;
        l->ultimo->next = NULL;
    }
}

void copiarLista(listaEncadeada *L, listaEncadeada *N) {

    criar(N);

    nodePt atualL = L->primeiro;
    while (atualL != NULL) {
        nodePt novoNo = new struct Node;
    
        novoNo->info = atualL->info;
        novoNo->next = NULL;

        if (N->primeiro == NULL) {
            N->primeiro = novoNo;
            N->ultimo = novoNo;
        } else {
            N->ultimo->next = novoNo;
            N->ultimo = novoNo;
        }

        atualL = atualL->next;
    }
}

void imprime(listaEncadeada *l){
    nodePt a = l->primeiro;
    while(a != NULL){
        cout << a->info << " ";
        a = a->next;
    }
    cout << endl << endl;
}

//--------------------------------------Criação da matriz--------------------------------------------------//
int** newMatriz(int tamanho){
    int** matriz = new int*[tamanho];       // Aloca memória para uma matriz tamanho x tamanho e retorna um ponteiro int** dela
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

    while (getline(arquivo, linha)) {
        ++linhas;
        istringstream iss(linha);
        int valor;
        while (iss >> valor) {
            ++colunas;
        }
    }

    // Reposiciona o cursor do arquivo no inicio
    arquivo.clear();
    arquivo.seekg(0);

    for (int i = 0; i < linhas; ++i) {                      // Pega a matriz salva no arquivo.txt do que está no caminho passado no nomeArquivo e copia para a matriz
        getline(arquivo, linha);                            //que foi passada como referencia na assinatura dessa funcao
        istringstream iss(linha);
        for (int j = 0; j < colunas / linhas; ++j) {
            int valor;
            iss >> valor;
            matriz[i][j] = valor;
        }
    }
}
//--------------------------------------Funções do Annealing---------------------------------------------//

float custo(listaEncadeada *l, int** matrizCusto){
    float soma = 0;
    nodePt a = l->primeiro;
    for (int i = 0; i < tamanho - 1; i++){
        soma += matrizCusto[a->info][a->next->info];
    }
    soma += matrizCusto[l->ultimo->info][l->primeiro->info];
    return soma;
}

float comparacao(listaEncadeada *l, listaEncadeada *possivelEstado, int** matrizCusto){//TESTAR
    return custo(possivelEstado, matrizCusto) - custo(l, matrizCusto);
}

float probabilidadeAceitar(float temp, float deltaC){//TESTAR
    return (exp(-1* deltaC / temp));
}

//--------------------------------------Criação de nova solução--------------------------------------------------//

void sortPosition(int *posA, int *posB){ //TESTAR
    *posA = (rand() % (tamanho - 1)) + 1;
    *posB = (rand() % (tamanho - 1)) + 1;

    while(*posA == *posB){
        *posA = (rand() % (tamanho - 1)) + 1;
        *posB = (rand() % (tamanho - 1)) + 1;
    }

    if(*posA > *posB){
        int aux = *posA;
        *posA = *posB;
        *posB = aux;
    }
}

void changeSolution(listaEncadeada *l, int posA, int posB){//TESTAR
    int func = rand() % 4;

    switch(func){
        case 0:
            inverse(l, posA, posB);
            break;
        case 1:
            swap(l, posA, posB);
            break;
        case 2:
            reinsert(l, posA, posB);
            break;
        case 3:
            int posC = (rand() % (tamanho - 1)) + 1;
            bool flag = true;
            while(flag){
                if (posA == 1 && posB == (tamanho - 1)){
                    sortPosition(&posA, &posB);
                }
                else{
                    if(posC == posA || posC == posB)
                       posC = (rand() % (tamanho - 1)) + 1; 
                    else
                        flag = false;
                }
            }
            reinsertSubRote(l, posA, posB, posC);
            break;
    }

}

void swap(listaEncadeada *l, int posA, int posB){

    int index = 0;

    nodePt iterator = l->primeiro;
    nodePt prevA;
    nodePt A;
    nodePt nextB;
    nodePt prevB;

        //Caso 1: posA e posB são adjacentes
    if (posB == posA + 1){
        reinsert(l, posA, posB);
    }   
    else{
        while (iterator != NULL){
            if (index == posA - 1)
                prevA = iterator;
            if (index == posB - 1)
                prevB = iterator;
            iterator = iterator->next;
            index++;  
    }
        A = prevA->next;
        nextB = prevB->next->next;
        prevA->next = prevB->next;
        prevB->next->next = A->next;
        prevB->next = A;
        A->next = nextB;
    }
}

void reinsert(listaEncadeada *l, int posA, int posB){
    int index = 0;

    nodePt iterator = l->primeiro;
    nodePt A;
    nodePt prevA;
    nodePt nextB;
    nodePt B;

    while (iterator != NULL){
        if(index == posA - 1)
            prevA = iterator;
        if(index == posB)
            B = iterator;
        index++;
        iterator = iterator->next;
    }

    A = prevA->next;
    nextB = B->next;
    
    prevA->next = A->next;
    B->next = A;
    A->next = nextB;
}


    //Talvez o inverse seja mais eficiente com uma lista duplamente encadeada, porém preciso fazer a análise se é realmente uma melhora significativa
void inverse(listaEncadeada *l, int posA, int posB){
    for (posA; posA < posB; posA++){
        swap(l, posA, posB);
        posB--;
    }
}

void reinsertSubRote(listaEncadeada *l, int posA, int posB, int posC){
    int index = 0;
    nodePt iterator = l->primeiro;

        //Caso 1: posC está após posB:
    if (posC > posB){
        nodePt prevA;
        nodePt A;
        nodePt B;
        nodePt C;
        nodePt nextC;

        while (iterator != NULL){
            if(index == posA -1)
                prevA = iterator;
            if(index == posB)
                B = iterator;
            if(index == posC)
                C = iterator;
            iterator = iterator->next;
            index++;
        }
        A = prevA->next;
        nextC = C->next;

        prevA->next = B->next;
        C->next = A;
        B->next = nextC;
    }

        //Caso 2: posC está antes de posA:
    //TESTAR CASO nextB == NULL
    if(posC < posA){   
        nodePt A;
        nodePt prevA;
        nodePt B;
        nodePt nextB;
        nodePt C;
        nodePt prevC;
        
        while (iterator != NULL){
            if (index == posA - 1)
                prevA = iterator;
            if(index == posB)
                B = iterator;
            if(index == posC - 1)
                prevC = iterator;
            iterator = iterator->next;
            index++;
        }
        A = prevA->next;
        nextB = B->next;
        C = prevC->next;

        prevC->next = A;
        B->next = C;
        prevA->next = nextB; 
    }

        //Caso 3: posC está entre posA e posB:
        //PENSAR POSSIVEIS RESTRIÇÕES
    if(posC > posA && posC < posB){
    
    }
}

//----------------------------------------O annealing em si------------------------------------------------//

void startAnnealing (listaEncadeada *l, int** matrizCusto, int* qntdSolucoesCriadas){
    listaEncadeada rotaMin;
    copiarLista(l, &rotaMin);
    int custoMin = custo(l, matrizCusto);

    int temperatura = temperaturaInicial;

    listaEncadeada possivelSolucao;
    copiarLista(l, &possivelSolucao);
    
    while(1 < temperatura){
        for(int i = 0; i<loopInterno; i++){
            int posA = 0, posB = 0;
            sortPosition(&posA, &posB);

            changeSolution(&possivelSolucao, posA, posB);

            float deltaC = comparacao(l, &possivelSolucao, matrizCusto);
            if(deltaC < 0)
                copiarLista(&possivelSolucao, l);
            else{
                if((rand() % 1000) < probabilidadeAceitar(temperatura, deltaC) * 1000)
                    copiarLista(&possivelSolucao, l);
            }

            if (custoMin > custo(l, matrizCusto)){
                copiarLista(l, &rotaMin);
                custoMin = custo(l, matrizCusto);
            }
        }
        temperatura = temperatura * alpha;
    }
    copiarLista(&rotaMin, l);
}
#endif