#include <iostream>
#include <cstdlib>
#include <ctime>
#define TAM 256

using namespace std;

void _buildMatrix(int mat[][TAM]){
	for(int i=0; i<TAM; i++){
		for (int j = 0; j<TAM; j++){
			if(i == 0 || j == 0 || i == TAM-1 || j == TAM-1){
				mat[i][j] = 0;
			} else{
				mat[i][j] = rand()%2;
			}
		}
	}
}

int _sumMask(int mat[][TAM], int m, int n){
	int temp = mat[m-1][n-1] + mat[m-1][n] + mat[m-1][n+1] + 
			mat[m][n-1] + mat[m][n+1] +
			mat[m+1][n+1] + mat[m+1][n] + mat[m+1][n-1];

	return temp;
}

void _attCopy(int temp, int mat[][TAM], int cmat[][TAM], int m, int n){
	if((temp == 2 || temp == 3) && mat[m][n] == 1){
		cmat[m][n] = 1;
	} else if (temp == 3 && mat[m][n] == 0){
		cmat[m][n] = 1;
	} else{
		cmat[m][n] = 0;
	}
}

int main(int argc, char **argv){
	int cmat[TAM][TAM], //Matriz T
		mat[TAM][TAM];	//Matriz T-1

	clock_t t0, t1; //Tempo de início de fim
	double total;		

	_buildMatrix(mat);

	t0 = clock();

	for(int m=1; m<TAM-1; m++){
		for (int n=1; n<TAM-1; n++){
			int temp = _sumMask(mat, m, n); //Calcula a máscara na posição m e n da matriz T-1
			_attCopy(temp, mat, cmat, m, n); //Joga o valor para a matriz T
		}
	}

	t1 = clock();

	total = ((double)(t1-t0))/CLOCKS_PER_SEC;

	cout << total << endl;

	return 0;
}