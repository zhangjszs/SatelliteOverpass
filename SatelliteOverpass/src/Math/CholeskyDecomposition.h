/***************************************************************************************
 Cholesky Decomposition

 Reference: Section 2.9, pp. 89-91, Numerical Recipes in Fortran, William Press, et al.

***************************************************************************************/
#pragma once

class cCholeskyDecom
{
public:

	cCholeskyDecom();
	~cCholeskyDecom();

	bool Decompose( double *pdfMatrix, int nDim );
	bool Decompose( double *pdfMatrix, int nDim, int nDim0 );
	bool LinearEquation( double *pdfMatrix, double *pdfB, int nDim, double *pdfX );
	bool InverseLowTriangular( double *pdfMatrix, int nDim );
	bool MatrixInversing( double *pdfMatrix, int nDim, int nCase );
	bool MatrixInversing( double *pdfMatrix, int nDim, int nDim0, int nCase );
};