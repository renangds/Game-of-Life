#include <iostream>
#include <cstdlib>
#include <ctime>
#include <mpi.h>
#define TAM 256
#define MASTER 0
#define TPROCESS 1

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

	int id_task, //Número do processo
		num_task, //Threads
		namelen, //Nome da máquina que o processador está sendo usado
		elmt_task, //Quantidade de elementos na comunicação
		tag_task, //Identificador da comunicação
		work_tasks; //Divisão da matriz por threads

	double t0, t1; //Tempo de início de fim		

	MPI_Status status;

	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&num_task);
	MPI_Comm_rank(MPI_COMM_WORLD,&id_task);
	MPI_Get_processor_name(processor_name,&namelen);

	_buildMatrix(mat);

	work_tasks = TAM/num_task;

	t0 = MPI_Wtime();

	if(id_task == MASTER){
		for(int m=1; m<TAM-1; m++){
			for (int n=1; n<(work_tasks*(id_task+1))-1; n++){
				int temp = _sumMask(mat, m, n); //Calcula a máscara na posição m e n da matriz T-1
				_attCopy(temp, mat, cmat, m, n); //Joga o valor para a matriz T
				if(n+1 == (work_tasks*(id_task+1))-1){
					MPI_Send(&mat[m][n+1], 1, MPI_INT, id_task+1, TPROCESS, MPI_COMM_WORLD); //Envia mensagem ao próximo processo				
				}
			}
		}
	} else if (id_task > MASTER){
		for(int m=1; m<TAM-1; m++){
			for (int n=work_tasks*id_task; n<(work_tasks*(id_task+1))-1; n++){
				int temp = _sumMask(mat, m, n); //Calcula a máscara na posição m e n da matriz T-1
				_attCopy(temp, mat, cmat, m, n); //Joga o valor para a matriz T

				if(n-1 == (work_tasks*id_task)-1){
					MPI_Recv(&mat[m][n-1], 1, MPI_INT, id_task-1, TPROCESS, MPI_COMM_WORLD, &status); //Recebe a mensagem do processo
				}
				
				//Generalização para mais de duas threads
				if((id_task < num_task) && (n == (work_tasks*(id_task+1))-1)){
					MPI_Send(&mat[m][n+1], 1, MPI_INT, id_task+1, TPROCESS, MPI_COMM_WORLD);
				}
			}
		}
	}

	t1 = MPI_Wtime();

	MPI_Finalize();

	cout << t1-t0 <<  endl;

	return EXIT_SUCCESS;
}