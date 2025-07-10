#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "matrix.h"

/* matrix[row][column] */

matrix *matalloc[MAXMAT];
int matnumalloc = 0;

matrix *matnew(int row, int col)
{
	matrix *temp = (matrix *) malloc(sizeof(matrix));
	if (temp==NULL || matnumalloc>=MAXMAT) return NULL;
	temp->row = row;
	temp->col = col;
	temp->data = (double *) malloc(sizeof(double)*row*col);
	matalloc[matnumalloc++] = temp;
	return temp;
}

double matget(matrix *target, int row, int col)
{
	if (target->row < row || target->col < col) {
		fprintf(stderr, "matrix: bad element access");
		return NAN;
	} return target->data[row*target->col + col];
}

void matput(matrix *target, int row, int col, double val)
{
	if (target->row < row || target->col < col) {
		fprintf(stderr, "matrix: bad element access");
		return;
	} target->data[row*target->col + col] = val;
	return;
}

void matmuleq(matrix *to, matrix *with)
{

	if (to->col != with->row) {
		fprintf(stderr, "matrix: mul operation not defined");
		return;
	}

	int n = to->col;

	matrix *temp = matnew(to->row, with->col);
	double sum;

	for (int r=0; r<to->row; r++) {
		for (int c=0; c<with->col; c++) {
			sum = 0.0;
			for (int k=0; k<n; k++) {
				sum += matget(to, r, k) *  matget(with, k, c);
			} matput(temp, r, c, sum);
		}
	}

	if (to->col != n) to->data = realloc(to->data, sizeof(double)*(with->col)*(to->row));

	to->col = with->col;
	for (int i=0; i<to->row; i++) {
		for (int j=0; j<to->col; j++) {
			matput(to, i, j, matget(temp, i, j));
		}
	}

	return;
}

void mataddeq(matrix *to, matrix *with)
{
	if (to->row != with->row || to->col != with->col) {
		fprintf(stderr, "matrix: add operation not defined");
		return;
	}
	for (int r=0; r<to->row; r++) {
		for (int c=0; c<to->col; c++) {
			matput(to, r, c, matget(to, r, c) + matget(with, r, c));
		}
	} return;
}

matrix *matmul(matrix *to, matrix *with)
{

	if (to->col != with->row) {
		fprintf(stderr, "matrix: mul operation not defined");
		return NULL;
	}

	int n = to->col;

	matrix *temp = matnew(to->row, with->col);
	double sum;

	for (int r=0; r<to->row; r++) {
		for (int c=0; c<with->col; c++) {
			sum = 0.0;
			for (int k=0; k<n; k++) {
				sum += matget(to, r, k) *  matget(with, k, c);
			} matput(temp, r, c, sum);
		}
	} return temp;
}

void mataddeq(matrix *to, matrix *with)
{
	if (to->row != with->row || to->col != with->col) {
		fprintf(stderr, "matrix: add operation not defined");
		return;
	}

	matrix *temp = matnew(to->row, to->col);

	for (int r=0; r<to->row; r++) {
		for (int c=0; c<to->col; c++) {
			matput(temp, r, c, matget(to, r, c) + matget(with, r, c));
		}
	} return temp;
}

void matfree()
{
	matrix *work;
	for (int k=0; k<matnumalloc; k++) {
		work = matalloc[k];
		free(work->data);
		free(work);
	}
	matnumalloc = 0;
	return;
}
