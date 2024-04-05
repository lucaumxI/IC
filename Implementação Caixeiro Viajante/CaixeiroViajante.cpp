#include <iostream>
#include <time.h> 
#include <bits/stdc++.h>
#include <fstream>
#include <sstream>


using namespace std;

//parametros iniciais

int tamanho = 48;          //quantidade de cidades
float temperatura = 4000;   //temperatura inicial
float alpha = 0.99;         //taxa de resfriamento
int loopInterno = 500;      //loop linha 168 (não pensei num nome melhor que loopInterno)
int minimo = 33523;          //custo minimo para o problema especifico que esta tentando resolver
int loopExterno = 1000;     //loop da linha 218 (não pensei num nome melhor que loopExterno)

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

void startRote(int* v){
    int inicio = rand() % (tamanho + 1);        // Sorteia uma cidade inicial e coloca ela no comeco e no final do vetor de tamanho (tamanho + 1)
    v[0] = inicio;                              // Usei (tamanho + 1) como o tamanho do vetor porque ter um indice a mais não muda nada na eficiencia
    v[tamanho] = inicio;                        // e estou gastando mais 3 int de memoria, não é nem um pouco relevante isso (creio eu) e eu achei melhor denotar a rota inteira

    int cont = 1;                               //insere as outras cidades em ordem crescente com excessão da cidade adicionada no passo acima
    for (int i = 0; cont < tamanho; i++){
        if(inicio != i){
            v[cont] = i;
            cont++;
        }
    }
}

float custo(int v[], int** matrizCusto){                                    //calcula o custo da rota passada como parametro para a funcao e retorna um float com o custo total
    float soma = 0;
    for (int cidadeAtual = 0; cidadeAtual < tamanho ; cidadeAtual++){
        soma += matrizCusto[v[cidadeAtual]][v[cidadeAtual + 1]];
    }

    return soma;        
}

void swap(int* v, int posA, int posB){      //troca o valor do indice posA com o valor do indice posB
    int aux = v[posA];                      //ex: 0 1 2 3 4 5 0 -> 0 1 4 3 2 5 0
    v[posA] = v[posB];
    v[posB] = aux;
}

void inverse(int* v, int posA, int posB){   //inverte a ordem do subvetor do vetor original que comeca na posA e termina na posB
    int aux;                                //ex: 0 1 2 3 4 5 0 -> 0 1 4 3 2 5 0
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

void insert(int* v, int posA, int posB){      //retira o elemento do indice posA e insere na posB reajeitando o vetor original
    int aux;                                  //ex: 0 1 2 3 4 5 0 -> 0 1 3 4 5 2 0

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

void changeSolution(int* v, int posA, int posB){        //sorteia um numero entre 0 e 2 para definir qual método irá gerar uma nova solucao
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

float comparacao(int* v, int* possivelEstado, int** matrizCusto){           //faz uma comparacao entre a solucao original e a possivel nova solucao e retorna
                                                                            //a diferenca entre o custo delas
    return custo(possivelEstado, matrizCusto) - custo(v, matrizCusto);
}

float probabilidadeAceitar(float temp, float deltaC){                       //calcula a probabilidade da nova solucao ser aceita

    return (exp(-1* deltaC / temp));
}

int* startAnneling(int v[], int** matrizCusto){
    int* rotaMin = new int[tamanho + 1];                //declara a rotaMin para ser armazenado a melhor solucao que for encontrada
    for (int i = 0; i<tamanho + 1; i++)
        rotaMin[i] = v[i];
    int custoMin = custo(v, matrizCusto);
    int mesmaSolucao = 0;

    int* possivelEstado = new int[tamanho + 1];     //declara possivelEstado para armazenar a nova possivel solucao
    for (int i = 0; i<tamanho + 1; i++)
        possivelEstado[i] = v[i];

    while (mesmaSolucao < loopInterno){                 //so sai desse loop caso uma mesmaSolucao for "escolhida" um numero loopInterno de vezes
        int posA = 0, posB = 0;                     //sorteia os indices que serao usados para criar a nova solucao
        while (posA == posB){                       // caso as posicoes sorteadas sejam iguais, sorteia de novo para garantir que a nova solucao seja diferente da atual
            posA = rand() % (tamanho - 1) + 1;
            posB = rand() % (tamanho - 1) + 1;
        }

        changeSolution(possivelEstado, posA, posB);                     //altera a rota possivelEstado para criar uma solucao nova

        float deltaC = comparacao (v, possivelEstado, matrizCusto);     //compara as rotas possivelEstado e v (poderia ter pensado num nome melhor) e armazena no float deltaC a diferenca entre elas
        if (deltaC < 0){                            //caso deltaC negativo, a nova solucao é melhor que a atual
            mesmaSolucao = 0;                       //zeramos o contador de mesmaSolucao
            for (int i = 0; i<tamanho + 1; i++)     //copia a solucao possivelEstado para a rota v
                v[i] = possivelEstado[i];
        }
        else{
            if ((rand() % 1000) < probabilidadeAceitar(temperatura, deltaC) * 1000){    //caso deltaC positivo sorteia um numero entre 0 e 999 e caso ele seja menor que a probabilidadeAceitar consideramos essa nova solucao
                mesmaSolucao = 0;                       //zera o contador de mesmaSolucao
                for (int i = 0; i<tamanho + 1; i++)     //copia a solucao possivelEstado para a rota v 
                    v[i] = possivelEstado[i];
            }
            else                    //caso deltaC positivo e o nao aceite a nova solucao
                mesmaSolucao += 1;  //soma um no contador de mesmaSolucao
            
        }
        
        
        if (custoMin > custo(v, matrizCusto)){          //caso a rota v seja melhor que a que esta armazenada no vetor custoMin
            for (int i = 0; i<tamanho + 1; i++){        //copiamos o vetor v para o custoMin
                rotaMin[i] = v[i];
                custoMin = custo(v, matrizCusto);       //atribui o novo custoMin
            }
        }
        temperatura = temperatura * alpha;

    }
    return rotaMin;                                     //retorna a rota de menor custo encontrada
}


int main(){
    srand(time(NULL));

    int** matrizCusto = newMatriz(tamanho);
    preencheMatriz(tamanho, matrizCusto);

    int* v = new int[tamanho + 1];
    
    startRote(v);

    for (int i = 0; i < loopExterno; i++)
        v = startAnneling(v, matrizCusto);

    cout << "Diferenca minimo achado pro real (porcentagem) = " << 100*(custo(v, matrizCusto) - minimo)/minimo << endl;
    cout << "Diferenca minimo achado pro real (numero absoluto)= " << custo(v, matrizCusto) - minimo << endl;
    cout<< "Rota minima: ";
    for (int i = 0; i<tamanho + 1; i++)
        cout << v[i] + 1 << " ";
    cout << endl << "Custo minimo: " << custo(v, matrizCusto) << endl;

    for (int i = 0; i < tamanho; ++i)
        delete[] matrizCusto[i];
    delete[] matrizCusto;

    return 0;
}