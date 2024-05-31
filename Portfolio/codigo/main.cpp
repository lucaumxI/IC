#include "annealing.cpp"

int main(){
    srand(time(NULL));

    int* v = startSolucao();
    /* double* preco = startVetorPreco();
    double* retorno = startVetorRetorno();
    double** covariancia = startMatrizCovariancia();

    v[0] = 2;
    v[4] = 1;
    v[19] = 98;

    cout << fixed << setprecision(17);
    for(int i = 0; i< qntAcoes; i++){
        if(v[i] > 0){
            cout << "Ação " << i << endl << "Preco: " << preco[i] << endl << "Retorno: " << retorno[i] << endl << "Qntd comprada: " << v[i] << endl << endl;
        }
    }

    cout << "Covariancia: " << endl << "0, 4: " << covariancia[0][4] << endl << "0, 19:" << covariancia[0][19] << endl << "4, 19: " << covariancia[4][19] << endl << endl;
    cout << lambda(preco,retorno, covariancia, v); */

    
    startAnnealing(v);
    for (int i = 0; i < qntAcoes; i++){
        if(v[i] != 0)
            cout << "Acao " << i << ": " <<  v[i] << " " << endl;
    }

    return 0;
}