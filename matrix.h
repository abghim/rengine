#ifndef _MATRIX_H_
#define _MATRIX_H_

#define MAXMAT 100

typedef struct {
    unsigned int row;
    unsigned int col;
    double *data;
} matrix;



matrix *matnew(int row, int col);
double matget(matrix *target, int row, int col);
void matput(matrix *target, int row, int col, double val);
void matmuleq(matrix *to, matrix *with);
void mataddeq(matrix *to, matrix *with);
matrix *matmul(matrix *to, matrix *with);
matrix *matadd(matrix *to, matrix *with);
void matfree();


#endif
