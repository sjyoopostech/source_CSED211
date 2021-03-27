/*
 * name : Yoo SeungJae
 * loginID : sjyoo
 */

/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
	int i, j, t;
	if (M == 32 && N == 32) {
		int bi, bj;
		for (bj = 0; bj < 32; bj += 8) {
			for (bi = 0; bi < 32; bi += 8) {
				for (i = bi; i < bi+8; i++) {
					for (j = bj; j < bj+8; j++) {
						if (i!=j) B[j][i] = A[i][j];
						else t = A[i][j];
					}
					if (bi==bj) B[i][i] = t;
				}
			}
		}
	}
	else if (M == 64 && N == 64) {
		int a, b, c, d, e, f, g, h;
		for (i = 0; i < 64; i += 8) {
			for (j = 0; j < 64; j += 8) {
				for (t = 0; t < 8; t++) {
					a = A[i+t][j];
					b = A[i+t][j+1];
					c = A[i+t][j+2];
					d = A[i+t][j+3];
					if (t == 0) {
						e = A[i][j+4];
						f = A[i][j+5];
						g = A[i][j+6];
						h = A[i][j+7];
					}
					B[j][i+t] = a;
					B[j+1][i+t] = b;
					B[j+2][i+t] = c;
					B[j+3][i+t] = d;
				}
				for (t = 7; t > 0; t--) {
					a = A[i+t][j+4];
					b = A[i+t][j+5];
					c = A[i+t][j+6];
					d = A[i+t][j+7];
					B[j+4][i+t] = a;
					B[j+5][i+t] = b;
					B[j+6][i+t] = c;
					B[j+7][i+t] = d;
				}
				B[j+4][i] = e;
				B[j+5][i] = f;
				B[j+6][i] = g;
				B[j+7][i] = h;
			}
		}
	}
	else if (M == 61 && N == 67) {
		int bi, bj;
		for (bj = 0; bj < 61; bj += 8) {
			for (bi = 0; bi < 67; bi += 8) {
				for (i = bi; (i < bi+8)&&(i<67); i++) {
					for (j = bj; (j < bj+8)&&(j<61); j++) {
						if (i!=j) B[j][i] = A[i][j];
						else t = A[i][j];
					}
					if (bi==bj) B[i][i] = t;
				}
			}
		}
	}
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

