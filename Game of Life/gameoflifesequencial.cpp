#include <iostream>
#include <cstdlib>
#include <ctime>
#define TAM 2048

using namespace std;

int* _buildMatrix(){
	int* mat = (int*)malloc(TAM*TAM*sizeof(int));

	for(int m=0; m<TAM; m++){
		for (int n = 0; n<TAM; n++){
			if(m == 0 || n == 0 || m == TAM-1 || n == TAM-1){
				mat[m*TAM+n] = 0;
			} else{
				mat[m*TAM+n] = rand()%2;
			}
		}
	}

	return mat;
}

int _sumMask(int* mat, int m, int n){
	int temp = mat[(m-1)*TAM+(n-1)] + mat[(m-1)*TAM+n] + mat[(m-1)*TAM+(n+1)] + 
			mat[(m)*TAM+(n-1)] + mat[m*TAM+(n+1)] +
			mat[(m+1)*TAM+(n+1)] + mat[(m+1)*TAM+n] + mat[(m+1)*TAM+(n-1)];

	return temp;
}

void _attCopy(int temp, int* mat, int* cmat, int m, int n){
	if((temp == 2 || temp == 3) && mat[m*TAM+n] == 1){
		cmat[m*TAM+n] = 1;
	} else if (temp == 3 && mat[m*TAM+n] == 0){
		cmat[m*TAM+n] = 1;
	} else{
		cmat[m*TAM+n] = 0;
	}
}

void print_matrix(int* mat){
	for(int m=0; m<TAM; m++){
		for (int n = 0; n<TAM; n++){
			cout << mat[m*TAM+n];
		}
		cout << endl;
	}
}

int main(int argc, char **argv){

	clock_t t0, t1; //Tempo de início de fim
	double total;

	int* mat = _buildMatrix();
	int* cmat = (int*)malloc(TAM*TAM*sizeof(int));

	t0 = clock();

	for(int m=1; m<TAM-1; m++){
		for (int n=1; n<TAM-1; n++){
			int temp = _sumMask(mat, m, n); //Calcula a máscara na posição m e n da matriz T-1
			_attCopy(temp, mat, cmat, m, n);
		}
	}

	t1 = clock();

	total = ((double)(t1-t0))/CLOCKS_PER_SEC;

	cout << total << endl;

	free(mat);
	free(cmat);

	return 0;
}