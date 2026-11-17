/***************************************************************************
 *
 * parallel version of Gaussian elimination
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#define min(a,b) ((a)<(b)?(a):(b)) // macro because c has no lib for min
#define MAX_SIZE 4096

typedef double matrix[MAX_SIZE][MAX_SIZE];

int	N;		/* matrix size		*/
int	maxnum;		/* max number of element*/
char	*Init;		/* matrix init type	*/
int	PRINT;		/* print switch		*/
matrix	A;		/* matrix A		*/
double	b[MAX_SIZE];	/* vector b             */
double	y[MAX_SIZE];	/* vector y             */

/* forward declarations */
void work(void);
void Init_Matrix(void);
void Print_Matrix(void);
void Init_Default(void);
int Read_Options(int, char **);

int
main(int argc, char **argv)
{
    int i, timestart, timeend, iter;

    Init_Default();		/* Init default values	*/
    Read_Options(argc,argv);	/* Read arguments	*/
    Init_Matrix();		/* Init the matrix	*/
    work();
    if (PRINT == 1)
	   Print_Matrix();
}

struct threadArgs { // start and end of rows for thread to handel aswell as pviot 
	int start;
	int end;
    int pivotK;
};

void* child(void* params) { // thread function
	struct threadArgs *args = (struct threadArgs*) params;
    int k = args->pivotK;
    for (int i = args->start; i < args->end; i++) { // elimination part from the work function moved to thread
	        for (int j = k+1; j < N; j++)
		        A[i][j] = A[i][j] - A[i][k]*A[k][j]; /* Elimination step */
	        b[i] = b[i] - A[i][k]*y[k];
	        A[i][k] = 0.0;
	    }
    return NULL;
}

void work(void)
{
    int i, j, k;

    int numThreads = 8; // 8 instead of 16 threads performes better
    pthread_t threads[numThreads];
    struct threadArgs args[numThreads];

    /* Gaussian elimination algorithm, Algo 8.4 from Grama */
    for (k = 0; k < N; k++) { /* Outer loop */
	    for (j = k+1; j < N; j++)
	       A[k][j] = A[k][j] / A[k][k]; /* Division step */
	    y[k] = b[k] / A[k][k];
	    A[k][k] = 1.0;
        int block = (N-(k+1) + numThreads - 1) / numThreads;
        if (N - (k+1) > 700) // i added a cut of for when thread overhead is not worth it anymore
            {
            for (size_t t = 0; t < numThreads; t++)
            {
                args[t].start = (k+1) + t*block;
                args[t].end = min(args[t].start + block, N);
                args[t].pivotK = k;
                pthread_create(&(threads[t]), // our handle for the child
                NULL, // attributes of the child
                child, // the function it should run
                (void*)&args[t]); // args to that function
            }
            for (unsigned int id = 0; id < numThreads; id++) { // waiting for all threads to be done
            pthread_join(threads[id], NULL );
            }
        }
        else{
            for (i = k+1; i < N; i++) {
	        for (j = k+1; j < N; j++)
		        A[i][j] = A[i][j] - A[i][k]*A[k][j]; /* Elimination step */
	        b[i] = b[i] - A[i][k]*y[k];
	        A[i][k] = 0.0;
	        }
        }
        
	    
    }
        
}


void
Init_Matrix()
{
    int i, j;

    printf("\nsize      = %dx%d ", N, N);
    printf("\nmaxnum    = %d \n", maxnum);
    printf("Init	  = %s \n", Init);
    printf("Initializing matrix...");

    if (strcmp(Init,"rand") == 0) {
        for (i = 0; i < N; i++){
            for (j = 0; j < N; j++) {
                if (i == j) /* diagonal dominance */
                    A[i][j] = (double)(rand() % maxnum) + 5.0;
                else
                    A[i][j] = (double)(rand() % maxnum) + 1.0;
            }
        }
    }
    if (strcmp(Init,"fast") == 0) {
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                if (i == j) /* diagonal dominance */
                    A[i][j] = 5.0;
                else
                    A[i][j] = 2.0;
            }
        }
    }

    /* Initialize vectors b and y */
    for (i = 0; i < N; i++) {
        b[i] = 2.0;
        y[i] = 1.0;
    }

    printf("done \n\n");
    if (PRINT == 1)
        Print_Matrix();
}

void
Print_Matrix()
{
    int i, j;

    printf("Matrix A:\n");
    for (i = 0; i < N; i++) {
        printf("[");
        for (j = 0; j < N; j++)
            printf(" %5.2f,", A[i][j]);
        printf("]\n");
    }
    printf("Vector b:\n[");
    for (j = 0; j < N; j++)
        printf(" %5.2f,", b[j]);
    printf("]\n");
    printf("Vector y:\n[");
    for (j = 0; j < N; j++)
        printf(" %5.2f,", y[j]);
    printf("]\n");
    printf("\n\n");
}

void
Init_Default()
{
    N = 2048;
    Init = "rand";
    maxnum = 15.0;
    PRINT = 0;
}

int
Read_Options(int argc, char **argv)
{
    char    *prog;

    prog = *argv;
    while (++argv, --argc > 0)
        if (**argv == '-')
            switch ( *++*argv ) {
                case 'n':
                    --argc;
                    N = atoi(*++argv);
                    break;
                case 'h':
                    printf("\nHELP: try sor -u \n\n");
                    exit(0);
                    break;
                case 'u':
                    printf("\nUsage: gaussian [-n problemsize]\n");
                    printf("           [-D] show default values \n");
                    printf("           [-h] help \n");
                    printf("           [-I init_type] fast/rand \n");
                    printf("           [-m maxnum] max random no \n");
                    printf("           [-P print_switch] 0/1 \n");
                    exit(0);
                    break;
                case 'D':
                    printf("\nDefault:  n         = %d ", N);
                    printf("\n          Init      = rand" );
                    printf("\n          maxnum    = 5 ");
                    printf("\n          P         = 0 \n\n");
                    exit(0);
                    break;
                case 'I':
                    --argc;
                    Init = *++argv;
                    break;
                case 'm':
                    --argc;
                    maxnum = atoi(*++argv);
                    break;
                case 'P':
                    --argc;
                    PRINT = atoi(*++argv);
                    break;
                default:
                    printf("%s: ignored option: -%s\n", prog, *argv);
                    printf("HELP: try %s -u \n\n", prog);
                    break;
            }
    return 0;
}
