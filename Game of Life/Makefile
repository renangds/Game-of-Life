all:
	mpic++ gameoflife.cpp -o gameoflife
	mpirun -np 2 gameoflife > temp1.txt
	g++ gameoflifesequencial.cpp -Wall
	./a.out > temp2.txt