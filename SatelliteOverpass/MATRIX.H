/**
 * @file Matrix.h
 * @brief 现代矩阵运算模块
 *
 * 提供高性能矩阵运算功能，支持多种矩阵操作：
 * - 矩阵基本运算 (加、减、乘、转置)
 * - 矩阵分解 (LU、Cholesky)
 * - 矩阵求逆
 * - 向量运算 (点积、叉积、范数)
 * - 特征值计算
 *
 * 使用现代C++特性优化性能和类型安全。
 *
 * @author Original: Jizhang Sang (2002)
 * @author Modernized: kerwin_zhang
 * @version 2.0.0
 * @date 2026-02-08
 */

#pragma once

#include <array>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <optional>
#include <numbers>

namespace SatelliteOverpass::Math
{

/**
 * @class MatrixException
 * @brief 矩阵运算异常类
 */
class MatrixException : public std::runtime_error
{
public:
    explicit MatrixException(const std::string& message)
        : std::runtime_error(message) {}

    MatrixException(const std::string& message, size_t row, size_t col)
        : std::runtime_error(message + " (Row: " + std::to_string(row)
                           + ", Col: " + std::to_string(col) + ")")
    {}
};

/**
 * @template<typename T, size_t M, size_t N>
 * @brief 固定大小矩阵模板类
 *
 * @tparam T 元素类型
 * @tparam M 行数
 * @tparam N 列数
 */
template<typename T, size_t M, size_t N>
class Matrix
{
public:
    static constexpr size_t Rows = M;
    static constexpr size_t Cols = N;
    static constexpr size_t Size = M * N;

    using RowType = std::array<T, N>;
    using DataType = std::array<RowType, M>;

    /**
     * @brief 默认构造函数
     */
    Matrix() noexcept : data_{} {}

    /**
     * @brief 标量初始化构造函数
     */
    explicit Matrix(T scalar) noexcept
    {
        data_.fill(RowType{});
        if constexpr (M == N) {
            for (size_t i = 0; i < M; ++i) {
                data_[i][i] = scalar;
            }
        }
    }

    /**
     * @brief 从一维数组构造
     */
    static Matrix fromArray(const std::array<T, Size>& arr) noexcept
    {
        Matrix result;
        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                result.data_[i][j] = arr[i * N + j];
            }
        }
        return result;
    }

    /**
     * @brief 获取元素引用
     */
    T& operator()(size_t row, size_t col) noexcept
    {
        return data_[row][col];
    }

    /**
     * @brief 获取元素常量引用
     */
    const T& operator()(size_t row, size_t col) const noexcept
    {
        return data_[row][col];
    }

    /**
     * @brief 获取行引用
     */
    RowType& row(size_t index) noexcept { return data_[index]; }
    const RowType& row(size_t index) const noexcept { return data_[index]; }

    /**
     * @brief 获取数据指针
     */
    T* data() noexcept { return data_[0].data(); }
    const T* data() const noexcept { return data_[0].data(); }

    /**
     * @brief 矩阵加法
     */
    Matrix operator+(const Matrix& other) const noexcept
    {
        Matrix result;
        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                result.data_[i][j] = data_[i][j] + other.data_[i][j];
            }
        }
        return result;
    }

    /**
     * @brief 矩阵减法
     */
    Matrix operator-(const Matrix& other) const noexcept
    {
        Matrix result;
        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                result.data_[i][j] = data_[i][j] - other.data_[i][j];
            }
        }
        return result;
    }

    /**
     * @brief 标量乘法
     */
    Matrix operator*(T scalar) const noexcept
    {
        Matrix result;
        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                result.data_[i][j] = data_[i][j] * scalar;
            }
        }
        return result;
    }

    /**
     * @brief 矩阵乘法
     */
    template<size_t P>
    Matrix<T, M, P> operator*(const Matrix<T, N, P>& other) const noexcept
    {
        Matrix<T, M, P> result{};
        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < P; ++j) {
                T sum = T{};
                for (size_t k = 0; k < N; ++k) {
                    sum += data_[i][k] * other(k, j);
                }
                result(i, j) = sum;
            }
        }
        return result;
    }

    /**
     * @brief 矩阵转置
     */
    Matrix<T, N, M> transpose() const noexcept
    {
        Matrix<T, N, M> result;
        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                result(j, i) = data_[i][j];
            }
        }
        return result;
    }

    /**
     * @brief 计算弗罗贝尼乌斯范数
     */
    T normFrobenius() const noexcept
    {
        T sum = T{};
        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                sum += data_[i][j] * data_[i][j];
            }
        }
        return std::sqrt(sum);
    }

    /**
     * @brief 设置为零矩阵
     */
    void setZero() noexcept
    {
        data_.fill(RowType{});
    }

    /**
     * @brief 设置为单位矩阵
     */
    void setIdentity() noexcept
    {
        setZero();
        if constexpr (M == N) {
            const size_t minSize = std::min(M, N);
            for (size_t i = 0; i < minSize; ++i) {
                data_[i][i] = T(1);
            }
        }
    }

    /**
     * @brief 检查是否为零矩阵
     */
    bool isZero() const noexcept
    {
        constexpr T eps = std::numeric_limits<T>::epsilon() * T(100);
        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                if (std::abs(data_[i][j]) > eps) {
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * @brief 检查是否为单位矩阵
     */
    bool isIdentity() const noexcept
    {
        if constexpr (M != N) {
            return false;
        }
        constexpr T eps = std::numeric_limits<T>::epsilon() * T(100);
        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                T expected = (i == j) ? T(1) : T(0);
                if (std::abs(data_[i][j] - expected) > eps) {
                    return false;
                }
            }
        }
        return true;
    }

private:
    DataType data_;
};

/**
 * @template<typename T, size_t N>
 * @brief 向量模板类
 *
 * @tparam T 元素类型
 * @tparam N 向量维度
 */
template<typename T, size_t N>
class Vector : public Matrix<T, N, 1>
{
public:
    using Base = Matrix<T, N, 1>;

    Vector() : Base() {}
    explicit Vector(T scalar) : Base(scalar) {}

    /**
     * @brief 从数组构造
     */
    static Vector fromArray(const std::array<T, N>& arr) noexcept
    {
        Vector result;
        for (size_t i = 0; i < N; ++i) {
            result(i, 0) = arr[i];
        }
        return result;
    }

    /**
     * @brief 元素访问
     */
    T& operator[](size_t index) noexcept { return (*this)(index, 0); }
    const T& operator[](size_t index) const noexcept { return (*this)(index, 0); }

    /**
     * @brief 点积
     */
    T dot(const Vector& other) const noexcept
    {
        T sum = T{};
        for (size_t i = 0; i < N; ++i) {
            sum += (*this)[i] * other[i];
        }
        return sum;
    }

    /**
     * @brief 叉积 (3维向量)
     */
    Vector cross(const Vector& other) const noexcept
    {
        static_assert(N == 3, "Cross product requires 3D vectors");
        return Vector{
            (*this)[1] * other[2] - (*this)[2] * other[1],
            (*this)[2] * other[0] - (*this)[0] * other[2],
            (*this)[0] * other[1] - (*this)[1] * other[0]
        };
    }

    /**
     * @brief 2-范数 (欧几里得范数)
     */
    T norm2() const noexcept
    {
        return std::sqrt(this->dot(*this));
    }

    /**
     * @brief 归一化
     */
    Vector normalize() const noexcept
    {
        const T n = norm2();
        if (n > std::numeric_limits<T>::epsilon()) {
            return *this * (T(1) / n);
        }
        return Vector{};
    }

    /**
     * @brief 欧几里得距离
     */
    T distance(const Vector& other) const noexcept
    {
        Vector diff = *this - other;
        return diff.norm2();
    }
};

/**
 * @class DynamicMatrix
 * @brief 动态大小矩阵类
 *
 * 用于运行时确定大小的矩阵运算
 */
template<typename T>
class DynamicMatrix
{
public:
    DynamicMatrix() : rows_(0), cols_(0) {}
    DynamicMatrix(size_t rows, size_t cols, T defaultValue = T{})
        : rows_(rows), cols_(cols)
    {
        data_.resize(rows * cols, defaultValue);
    }

    DynamicMatrix(size_t rows, size_t cols, std::initializer_list<T> values)
        : rows_(rows), cols_(cols)
    {
        data_ = std::vector<T>(values);
        if (data_.size() != rows * cols) {
            throw MatrixException("Initializer list size mismatch");
        }
    }

    T& operator()(size_t row, size_t col)
    {
        return data_[row * cols_ + col];
    }
    const T& operator()(size_t row, size_t col) const
    {
        return data_[row * cols_ + col];
    }

    size_t rows() const noexcept { return rows_; }
    size_t cols() const noexcept { return cols_; }
    size_t size() const noexcept { return rows_ * cols_; }

    void resize(size_t rows, size_t cols)
    {
        rows_ = rows;
        cols_ = cols;
        data_.resize(rows * cols);
    }

    T* data() noexcept { return data_.data(); }
    const T* data() const noexcept { return data_.data(); }

private:
    size_t rows_;
    size_t cols_;
    std::vector<T> data_;
};

/**
 * @class MatrixOperations
 * @brief 静态矩阵运算类
 *
 * 提供各种矩阵运算功能
 */
template<typename T, size_t N>
class MatrixOperations
{
public:
    /**
     * @brief Cholesky分解 (A = L * L^T)
     * @return 下三角矩阵 L
     */
    static Matrix<T, N, N> cholesky(const Matrix<T, N, N>& A)
    {
        Matrix<T, N, N> L{};
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j <= i; ++j) {
                T sum = T{};
                for (size_t k = 0; k < j; ++k) {
                    sum += L(j, k) * L(j, k);
                }

                if (i == j) {
                    if (A(i, i) <= sum) {
                        throw MatrixException("Matrix is not positive definite", i, j);
                    }
                    L(i, j) = std::sqrt(A(i, i) - sum);
                } else {
                    if (std::abs(L(j, j)) < std::numeric_limits<T>::epsilon()) {
                        throw MatrixException("Division by zero in Cholesky", i, j);
                    }
                    T sum2 = T{};
                    for (size_t k = 0; k < j; ++k) {
                        sum2 += L(i, k) * L(j, k);
                    }
                    L(i, j) = (A(i, j) - sum2) / L(j, j);
                }
            }
        }
        return L;
    }

    /**
     * @brief LU分解 (带部分主元选择)
     * @return LU矩阵 (下三角和上三角共用)
     */
    static Matrix<T, N, N> lu(const Matrix<T, N, N>& A)
    {
        Matrix<T, N, N> LU = A;
        std::array<size_t, N> pivot{};
        for (size_t i = 0; i < N; ++i) pivot[i] = i;

        for (size_t k = 0; k < N; ++k) {
            size_t maxRow = k;
            T maxVal = std::abs(LU(k, k));
            for (size_t i = k + 1; i < N; ++i) {
                if (std::abs(LU(i, k)) > maxVal) {
                    maxVal = std::abs(LU(i, k));
                    maxRow = i;
                }
            }

            if (maxVal < std::numeric_limits<T>::epsilon()) {
                throw MatrixException("Singular matrix in LU decomposition", k, k);
            }

            if (maxRow != k) {
                std::swap(LU.row(k), LU.row(maxRow));
                std::swap(pivot[k], pivot[maxRow]);
            }

            for (size_t i = k + 1; i < N; ++i) {
                T factor = LU(i, k) / LU(k, k);
                LU(i, k) = factor;
                if (factor != T{}) {
                    for (size_t j = k + 1; j < N; ++j) {
                        LU(i, j) -= factor * LU(k, j);
                    }
                }
            }
        }
        return LU;
    }

    /**
     * @brief 使用LU分解求逆
     */
    static Matrix<T, N, N> inverse(const Matrix<T, N, N>& A)
    {
        const auto LU = lu(A);
        Matrix<T, N, N> inv{};

        for (size_t j = 0; j < N; ++j) {
            Vector<T, N> b{};
            b[j] = T(1);
            Vector<T, N> x = forwardSubstitution(LU, b);
            Vector<T, N> y = backwardSubstitution(LU, x);
            for (size_t i = 0; i < N; ++i) {
                inv(i, j) = y[i];
            }
        }
        return inv;
    }

    /**
     * @brief 前向替换 (Ly = b)
     */
    static Vector<T, N> forwardSubstitution(
        const Matrix<T, N, N>& L,
        const Vector<T, N>& b) noexcept
    {
        Vector<T, N> y{};
        for (size_t i = 0; i < N; ++i) {
            T sum = b[i];
            for (size_t j = 0; j < i; ++j) {
                sum -= L(i, j) * y[j];
            }
            if (std::abs(L(i, i)) < std::numeric_limits<T>::epsilon()) {
                throw MatrixException("Zero diagonal in forward substitution", i, i);
            }
            y[i] = sum / L(i, i);
        }
        return y;
    }

    /**
     * @brief 后向替换 (Ux = y)
     */
    static Vector<T, N> backwardSubstitution(
        const Matrix<T, N, N>& U,
        const Vector<T, N>& y) noexcept
    {
        Vector<T, N> x{};
        for (int i = static_cast<int>(N) - 1; i >= 0; --i) {
            T sum = y[i];
            for (size_t j = i + 1; j < N; ++j) {
                sum -= U(i, j) * x[j];
            }
            if (std::abs(U(i, i)) < std::numeric_limits<T>::epsilon()) {
                throw MatrixException("Zero diagonal in backward substitution", i, i);
            }
            x[i] = sum / U(i, i);
        }
        return x;
    }

    /**
     * @brief 计算特征值 (对称矩阵，QR算法)
     */
    static std::array<T, N> eigenvalues(const Matrix<T, N, N>& A)
    {
        Matrix<T, N, N> H = A;
        const size_t maxIter = 100;
        constexpr T tolerance = std::numeric_limits<T>::epsilon() * T(1000);

        for (size_t iter = 0; iter < maxIter; ++iter) {
            T maxOffDiag = T{};
            size_t p = 0, q = 1;

            for (size_t i = 0; i < N - 1; ++i) {
                for (size_t j = i + 1; j < N; ++j) {
                    if (std::abs(H(i, j)) > maxOffDiag) {
                        maxOffDiag = std::abs(H(i, j));
                        p = i;
                        q = j;
                    }
                }
            }

            if (maxOffDiag < tolerance) break;

            // Jacobi旋转
            T theta = T{};
            if (std::abs(H(p, p) - H(q, q)) < tolerance) {
                theta = std::numbers::pi / T(4);
            } else {
                theta = T(0.5) * std::atan2(T(2) * H(p, q), H(p, p) - H(q, q));
            }

            T c = std::cos(theta);
            T s = std::sin(theta);

            // 更新H矩阵
            T Hpp = H(p, p);
            T Hqq = H(q, q);
            T Hpq = H(p, q);

            H(p, p) = c * c * Hpp - T(2) * c * s * Hpq + s * s * Hqq;
            H(q, q) = s * s * Hpp + T(2) * c * s * Hpq + c * c * Hqq;
            H(p, q) = T{};
            H(q, p) = T{};

            for (size_t j = 0; j < N; ++j) {
                if (j != p && j != q) {
                    T Hjp = H(j, p);
                    T Hjq = H(j, q);
                    H(j, p) = c * Hjp - s * Hjq;
                    H(p, j) = H(j, p);
                    H(j, q) = s * Hjp + c * Hjq;
                    H(q, j) = H(j, q);
                }
            }
        }

        std::array<T, N> eigenvalues{};
        for (size_t i = 0; i < N; ++i) {
            eigenvalues[i] = H(i, i);
        }
        return eigenvalues;
    }
};

/**
 * @brief 便捷类型别名
 */
template<size_t N> using VectorNd = Vector<double, N>;
template<size_t M, size_t N> using MatrixMxNd = Matrix<double, M, N>;
using Vector2d = Vector<double, 2>;
using Vector3d = Vector<double, 3>;
using Matrix2d = Matrix<double, 2, 2>;
using Matrix3d = Matrix<double, 3, 3>;
using Matrix4d = Matrix<double, 4, 4>;

} // namespace SatelliteOverpass::Math