#include "annealing.cpp"

void imprimeVetorSolucoes(){
    ofstream file;
    file.open("warmvector.txt", ios::app);
    caminho = "database\\outros ai pra eu testar\\may\\24_may/retorno.txt";
    int* v;
    double* retorno = startVetorRetorno();
    double* preco = startVetorPreco();
    double** covariancia = startMatrizCovariancia();
    for (int j = 0; j<10; j++){
        v = startSolucao();
        startAnnealing(v, preco, retorno, covariancia);
        for (int i = 0; i<qntAcoes; i++){
            file<< v[i] <<" ";
        }
        file << endl;
    }

    file.close();
}

int main(){                 //testar mudando a aversão primeiro com 0 depois vai variando conforme o gosto do fregues
    srand(time(NULL));

    imprimeVetorSolucoes();


    return 0;
}