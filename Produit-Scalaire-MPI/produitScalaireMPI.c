#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

#define MIN -500000
#define MAX 500000

void generate_vector(float *vector, int tail){
    int i;
    for(i = 0; i<tail; i++){
        vector[i] = ((rand() % (MAX - MIN + 1)) + MIN);//*0.15;
    }
}

float produit_scalaire(float *vector1, float *vector2, unsigned int start, unsigned int tail){
    int k;
    float Pscalaire = 0.0;
    for(k = start; k<tail; k++){
        Pscalaire += vector1[k] * vector2[k];
    }
    return Pscalaire;
}

void print_vector(float *vector, int start, int tail){
    int k;
    for(k = start; k<tail; k++){
        printf("%.1f  ",vector[k]);
    }
    printf("\n");
}

int main(int argc, char *argv[]){ 

    srand(time(NULL));
    int tail = atoi(argv[1]);
    float *vectA = NULL;
    float *vectB = NULL;
    int rank, size;

    vectA = malloc(tail*sizeof(float));
    vectB = malloc(tail*sizeof(float));

    if (vectA == NULL || vectB == NULL){
        printf("UNE ERREUR S'EST PRODUITE!!");
        exit(0);
    }

    MPI_Init(&argc, &argv) ;
    MPI_Comm_rank (MPI_COMM_WORLD, &rank) ;
    MPI_Comm_size (MPI_COMM_WORLD, &size) ;

    if (rank == 0){
        // création des deux vecteurs
        generate_vector(vectA, tail);
        generate_vector(vectB, tail);

        //print_vector(vectA, 0, tail);
        //print_vector(vectB, 0, tail);
    }

    double starttime, endtime;
    starttime = MPI_Wtime();
    float *result = malloc(sizeof(float));
    float *global_result = malloc(sizeof(float));
    int sendCount = floor(tail/size) + 1;
    float rcvBufferA[sendCount];
    float rcvBufferB[sendCount];
 
    MPI_Scatter(vectA, sendCount, MPI_FLOAT, rcvBufferA, sendCount, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Scatter(vectB, sendCount, MPI_FLOAT, rcvBufferB, sendCount, MPI_FLOAT, 0, MPI_COMM_WORLD);

    //print_vector(rcvBufferA, 0, sendCount);
    //print_vector(rcvBufferB, 0, sendCount);
    *result = produit_scalaire(rcvBufferA, rcvBufferB, 0, sendCount);
    //printf("process rank = %d result = %f\n",rank,*result);
   	MPI_Reduce(result, global_result, size, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    endtime = MPI_Wtime();

    if (rank == 0){
        printf("Temps MPI =  %f ms\n",(endtime-starttime)*1000);
        free(vectA);
        free(vectB);
    }
    MPI_Finalize();
    return 0;
}
