/*****************************************************************************

  Matrix computation (legacy)

******************************************************************************/
#pragma once

#include <windows.h>

class cMatrix
{
public:
    cMatrix();
    ~cMatrix();

    static void Vec_Assign( double *Source, double *Result, int n );
    static void Set_Vec_Zero( double *Vec, int n );
    static void Vec_Multiply_By_Constant( double *Vec, int n, double c );
    static void Vec_Multiply_By_Constant( double *Vec, int n, double c, double *ResultVec );
    static void Vec_Minus_Vec( double *Vec1, double *Vec2, int n, double *ResultVec );

    static void TriMat_Multiply_By_Constant( double *TriMat, int n, double c );
    static void Mat_Multiply_By_Constant( double *Mat, int m, int n, double c );

    static bool Mat_Transpose( double *Mat, int m, int n );
    static bool UpperTriMat_Transpose( double *TriMat, int n );
    static bool LowerTriMat_Transpose( double *TriMat, int n );
    static bool TriMat_Inverse( double *TriMat, int n );
    static bool Mat_Inverse( double *Mat, int n );

    static double Vec_Multiply_Vec( double *Vec1, double *Vec2, int n );
    static void Vec_Multiply_Vec_To_Mat( double *Vec1, double *Vec2, double *Result, int m, int n );
    static void Vec_Multiply_Vec_To_TriMat( double *Vec, double *Result, int n );
    static void Vec_Multiply_TriMat( double *Vec, double *TriMat, double *Result, int n );
    static void Vec_Multiply_Mat( double *Vec, double *Mat, double *Result, int m, int n );
    static void TriMat_Multiply_Vec( double *TriMat, double *Vec, double *Result, int n );
    static void Mat_Multiply_Vec( double *Mat, double *Vec, double *Result, int m, int n );
    static void TriMat_Multiply_TriMat( double *TriMat1, double *TriMat2, double *Result, int n );
    static void TriMat_Multiply_Mat( double *TriMat, double *Mat, double *Result, int m, int n );
    static void Mat_Multiply_TriMat( double *Mat, double *TriMat, double *Result, int m, int n );
    static void Mat_Multiply_Mat( double *Mat1, double *Mat2, double *Result, int m, int n, int l );
    static bool LowerTriMat_Multiply_LowerTriMat( double *LowerTriMat1, double *LowerTriMat2, double *Result, int n );
    static bool UpperTriMat_Multiply_UpperTriMat( double *UpperTriMat1, double *UpperTriMat2, double *Result, int n );

    static void Mat_Add_Mat( double *Mat1, double *Mat2, double *Result, int m, int n );
    static void Mat_Sub_Mat( double *Mat1, double *Mat2, double *Result, int m, int n );
    static void Mat_Trianglise( double *Mat, double *Result_TriMat, int n );
    static void TriMat2Mat( double *TriMat, double *Result_Mat, int n );

    static bool PartitionedMatrixInverse( double *p11, double *p12, double *p22, int n1, int n2 );
    static void GaussianJordan( double *a, double *b, int n );
    static void VectorProduct( double *pdfVec1, double *pdfVec2, double *pdfVec3 );
    static bool EigensSymmMatrix( double *TriMat, int n, double *Eigenvalue, double *Eigenvector, bool bValueOnly );

    static double pythag( double a, double b );
    static double Vec_norm2( double *Vec, int n );
    static double Vec_Norm( double *Vec1, int n );
};

extern cMatrix g_Matrix;
