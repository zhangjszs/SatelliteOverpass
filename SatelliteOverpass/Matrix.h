/*****************************************************************************

  Matrix computation header file.

******************************************************************************/

#pragma once

class cMatrix
{
public:
    cMatrix();
    ~cMatrix();

    // Vector operations
    void Vec_Assign(double *Source, double *Result, int n);
    void Set_Vec_Zero(double *Vec, int n);
    void Vec_Multiply_By_Constant(double *Vec, int n, double c);
    void Vec_Multiply_By_Constant(double *Vec, int n, double c, double *ResultVec);
    double Vec_Multiply_Vec(double *Vec1, double *Vec2, int n);
    void Vec_Multiply_Vec_To_Mat(double *Vec1, double *Vec2, double *Mat, int n1, int n2);
    void Vec_Multiply_Vec_To_TriMat(double *Vec, double *TriMat, int n);
    void Vec_Multiply_TriMat(double *Vec, double *TriMat, double *Result, int n);
    void Vec_Multiply_Mat(double *Vec, double *Mat, double *Result, int n, int m);
    void Vec_Minus_Vec(double *Vec1, double *Vec2, int n, double *ResultVec);
    double Vec_norm2(double *Vec, int n);
    double Vec_Norm(double* Vec1, int n);

    // Matrix operations
    bool Mat_Transpose(double *Mat, int m, int n);
    bool UpperTriMat_Transpose(double *TriMat, int n);
    bool LowerTriMat_Transpose(double *TriMat, int n);
    bool TriMat_Inverse(double *TriMat, int n);
    bool Mat_Inverse(double *Mat, int n);
    void Mat_Multiply_By_Constant(double *Mat, int m, int n, double c);
    void Mat_Add_Mat(double *Mat1, double *Mat2, double *Result, int m, int n);
    void Mat_Sub_Mat(double *Mat1, double *Mat2, double *Result, int m, int n);
    void Mat_Trianglise(double *Mat, double *Result_TriMat, int n);
    void TriMat2Mat(double *TriMat, double *Result_Mat, int n);
    bool PartitionedMatrixInverse(double *p11, double *p12, double *p22, int n1, int n2);
    void GaussianJordan(double *a, double *b, int n);

    // Triangular matrix operations
    void TriMat_Multiply_By_Constant(double *TriMat, int n, double c);
    void TriMat_Multiply_Vec(double *TriMat, double *Vec, double *Result, int n);
    void TriMat_Multiply_TriMat(double *TriMat1, double *TriMat2, double *Result, int n);
    void TriMat_Multiply_Mat(double *TriMat, double *Mat, double *Result, int n, int m);
    void Mat_Multiply_TriMat(double *Mat, double *TriMat, double *Result, int m, int n);
    bool LowerTriMat_Multiply_LowerTriMat(double *LowerTriMat1, double *LowerTriMat2, double *LowerTriResult, int n);
    bool UpperTriMat_Multiply_UpperTriMat(double *UpperTriMat1, double *UpperTriMat2, double *UpperTriResult, int n);

    // Matrix multiplication
    void Mat_Multiply_Mat(double *Mat1, double *Mat2, double *Result, int m, int n, int p);
    void Mat_Multiply_Vec(double *Mat, double *Vec, double *Result, int m, int n);

    // Vector product
    void VectorProduct(double *pdfVec1, double *pdfVec2, double *pdfVec3);

    // Eigenvalues
    bool EigensSymmMatrix(double *TriMat, int n, double *evalues, double *evectors, bool bValueOnly);

    // Utility functions
    double pythag(double a, double b);
};

// Global matrix instance
extern cMatrix g_Matrix;