#include <iostream>
#include <cstdlib>
#include <ctime>
#include <mpi.h>
#define TAM 2048
#define MASTER 0
#define TPROCESS 1

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

int main(int argc, char **argv){

	int* mat = _buildMatrix();
	int* cmat = (int*)malloc(TAM*TAM*sizeof(int));

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

	work_tasks = TAM/num_task;

	t0 = MPI_Wtime();

	if(id_task == MASTER){
		for(int m=1; m<TAM-1; m++){
			for (int n=1; n<(work_tasks*(id_task+1)); n++){

				if((id_task == num_task) && (n+1 == (work_tasks*(id_task+1)))) break;

				if(n+1 == (work_tasks*(id_task+1))){
					MPI_Recv(&mat[m*TAM+(n+1)], 1, MPI_INT, id_task+1, TPROCESS, MPI_COMM_WORLD, &status);
				}

				int temp = _sumMask(mat, m, n); //Calcula a máscara na posição m e n da matriz T-1
				_attCopy(temp, mat, cmat, m, n); //Joga o valor para a matriz T

				if(n+1 == (work_tasks*(id_task+1))-1){
					MPI_Send(&mat[m*TAM+(n+1)] , 1, MPI_INT, id_task+1, TPROCESS, MPI_COMM_WORLD); //Envia mensagem ao próximo processo				
				}
			}
		}

	} else if (id_task > MASTER){
		for(int m=1; m<TAM-1; m++){
			for (int n=work_tasks*id_task; n<(work_tasks*(id_task+1))-1; n++){
				int temp = _sumMask(mat, m, n); //Calcula a máscara na posição m e n da matriz T-1
				_attCopy(temp, mat, cmat, m, n); //Joga o valor para a matriz T

				if(n == work_tasks*id_task){
					MPI_Send(&mat[m*TAM+n], 1, MPI_INT, id_task-1, TPROCESS, MPI_COMM_WORLD);
				}

				if(n-1 == (work_tasks*id_task)-1){
					MPI_Recv(&mat[m*TAM+(n-1)], 1, MPI_INT, id_task-1, TPROCESS, MPI_COMM_WORLD, &status); //Recebe a mensagem do processo
				}
				
				//Generalização para mais de duas threads
				if((id_task < num_task) && (n == (work_tasks*(id_task+1))-1)){
					MPI_Send(&mat[m*TAM+(n+1)], 1, MPI_INT, id_task+1, TPROCESS, MPI_COMM_WORLD);
				}
			}
		}
	}

	t1 = MPI_Wtime();

	MPI_Finalize();

	free(cmat);
	free(mat);

	cout << t1-t0 <<  endl;

	return EXIT_SUCCESS;
}