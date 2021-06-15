#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>


#define N 1E8  //100 milhões de vezes em valores inteiros, será utilizado no laço for
#define d 1E-8 //utilizado para transformação em ponto flutuante para multiplicação pelo valor inteiro da aproximação de pi

int main (int argc, char* argv[])
{
    int rank;
	int size;
	int i;
	int result=0, sum=0;
	int error;
	int namelen; 
	int z;
	
    double pi=0.0;
	double x, y;
    double start=0.0;
	double end=0.0; 
	
    
    error=MPI_Init (&argc, &argv);
    
    
    MPI_Comm_rank (MPI_COMM_WORLD, &rank); //retorna o rank (process id) do processo 
    MPI_Comm_size (MPI_COMM_WORLD, &size); //retorna o numero total de processos 
	
    // Pega o nome do nó em que o processo está sendo realizado
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Get_processor_name(processor_name, &namelen);

    printf("Processor %s, rank %d out of %d processors\n", processor_name, rank, size);//"printa" processo, rank e quantidade de processos
    
    MPI_Barrier(MPI_COMM_WORLD); //sincroniza os processos para realizarem a mesma parte do codigo

    start = MPI_Wtime();//contagem de tempo começa
    srand((int)time(NULL));//começa a "rolagem" de numeros aleatórios
    
    //Cada processo calcula parte da soma dos pontos
    //para cada rank(posição do processo), soma-se o valor total de divisões do processo para que nunca realize o mesmo calculo que outro nó
    for (i=rank; i<N; i+=size)
    {
        x=rand()/(RAND_MAX+1.0);
        y=rand()/(RAND_MAX+1.0);
		z = x*x+y*y;
        if(z<1.0) 	//condição que verifica se o ponto x e y estiver dentro do circulo
        	result++; 
    }
    
    MPI_Reduce(&result, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);//coloca o valor resultante no ˜buffer do processo especificado para que o resultado seja computado em somente uma variavel.
    
    MPI_Barrier(MPI_COMM_WORLD); //sincroniza os processos 
    
    end = MPI_Wtime(); //termina a contagem do tempo
    
    
    if (rank==0) //calculo do PI no nó mestre
    {
        pi=4*d*sum; //pi = multiplicação do quadrante calculado por 4
        printf("np=%2d; Time=%fs; PI=%0.6f\n", size, end-start, pi); 
    }
    
    error=MPI_Finalize();
    
    return 0;
}
