#include <iostream>
#include <math.h>

using namespace std;


void swap(int v[], int i, int j){
    int aux;
    aux = v[i];
    v[i] = v[j];
    v[j] = aux;
}

int partition(int v[], int left, int right){
    int pivo = v[left];
    int i = left;

    for (int j = left + 1; j <= right; j++){
        if (v[j] <= pivo){
            i++;
            swap(v, i, j);
        }
    }

    swap(v, left, i);
    return i;
}

void quickSort(int v[], int left, int right){
    if (left < right){
        int index = partition(v, left, right);
        quickSort(v, left, index - 1);
		quickSort(v, index + 1, right);	
    }
}



int buscaBinaria(int v[], int left, int right, int num){  
    int mid = floor((right + left)/2);
    
    if (right < left)
        return -1;

    if(v[mid] == num)
        return mid;
    
    if (v[mid] < num)
        return buscaBinaria(v, mid + 1, right, num);
    else
        return buscaBinaria(v, left, mid - 1, num);
}

int main(){
    int tam;
    cin >> tam;
    int v[tam];

    for(int i = 0; i<tam; i++){    
        cin >> v[i]; 
    }; 

    quickSort(v, 0, tam);

    int num;
    cin >> num;

    int index = buscaBinaria(v, 0, tam - 1, num);
    cout << index;
    return 0;
}