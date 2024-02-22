#include <iostream>

using namespace std;

int buscaSequencial(int v[], int tam, int num){  
    for(int i = 0; i<tam; i++){
        if (v[i] == num)
            return i;       //retorna o indice em que o numero procurado num está
    }
    return -1; // caso o elemento não esteja no vetor
}

int main(){
    int tam;    //cria vetor de tamanho tam
    cin >> tam;
    int v[tam];

    for(int i = 0; i<tam; i++){     //preenche o vetor de tamanho tam
        cin >> v[i]; 
    }; 
    int num;
    cin >> num;     //da input de qual numero você quer buscar
    int index = buscaSequencial(v, tam, num);       //chama a funcao buscaSequencial

    cout << index;      // imprime o indice onde esta o numero que procurou

    return 0;
}