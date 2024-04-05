#include <iostream>
#include <time.h>

using namespace std;

int tamanho = 5;

void startRote(int* v){
    int inicio = rand() % (tamanho + 1);
    v[0] = inicio;
    v[tamanho] = inicio;

    int cont = 1;
    for (int i = 0; cont < tamanho; i++){
        if(inicio != i){
            v[cont] = i;
            cont++;
        }
    }
}

int main(){
    srand(time(NULL));
    int* v = new int[6];

    startRote(v);

    for (int j = 0; j<50; j++){
        for (int i = 0; i < tamanho + 1; i++)
            cout << v[i] << " ";
        cout << endl;
         startRote(v);
    }
}   