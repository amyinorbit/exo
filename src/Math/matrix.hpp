//
//  Mat.h
//  Meteor
//
//  Created by Amy Parent <amy@amyparent.com> on 26/08/2015.
//  Copyright © 2015 Amy Parent. All rights reserved.
//

#pragma once

#include <iostream>
#include <initializer_list>

/*!
 * @class       Mat
 * @ingroup     Geometry
 * @brief       A class of objects used to represent two-dimensional matrices of arbitrary sizes.
 * @details     Matrices can be added, subtracted and multiplied together, as well as by scalars.
 *              The API in AffineTransform uses matrices to transform vectors and other basic
 *              geometric constructs.
 * @tparam      rows            The number of rows in the Matrix.
 * @tparam      cols            The number of columns in the Matrix.
 */
template<int rows, int cols>
struct matrix {
    
    /**
     * @name        Creating Matrices
     * @{
     */
    
    /*!
     * @brief       Creates a new matrix whose components are initialized to a single value.
     * @param       value       Value to set the matrix's components to.
     */
    matrix(long double value = 0) {
        for(int i = 0; i < rows; ++i) {
            std::fill_n(data[i], cols, value);
        }
    }
    
    /*!
     * @brief       Creates a matrix from a list of values.
     * @note        Values should be ordered by column, and then by line.
     * @param       values      Braced list of values to build the matrix from.
     */
    matrix(std::initializer_list<long double> values) {
        if(values.size() != rows*cols) {
            throw std::runtime_error("Wrong number of matrix components");
        }
        int i = 0;
        for(float v : values) {
            data[i/cols][i%cols] = v;
            ++i;
        }
    }
    
    /*!
     * @brief       Creates the identity matrix, with `1`s in the diagonal and `0`s elsewhere.
     * @verbatim
     1    0    0   ...
     
     0    1    0   ...
     
     0    0    1   ...
     ...  ...  ...
     @endverbatim
     * @return      The identity matrix.
     */
    static matrix
    identity() {
        matrix m;
        for(int i = 0; i < rows; ++i) {
            for(int j = 0; j < cols; ++j) {
                m[i][j] = (i == j) ? 1 : 0;
            }
        }
        return m;
    }
    
    static matrix
    null() {
        return matrix{NAN};
    }
    
    /*!
     * @brief       Returns the Transpose of the matrix.
     * @return      The transpose of the matrix.
     */
    matrix<cols, rows>
    transpose() const {
        matrix<cols, rows> m;
        for(int i = 0; i < rows; ++i) {
            for(int j = 0; j < cols; ++j) {
                m[j][i] = data[i][j];
            }
        }
        return m;
    }
    
    long double
    determinant() const {
        return 0;
    }
    
    /**
     * @}
     */
    
    /*!
     * @internal
     * @brief       Returns a given row from the matrix.
     * @param       row         Matrix's row to return.
     * @return      Pointer to the row's first index. Will be invalidated if the matrix is
     *              deleted from memory.
     */
    long double*
    operator[] (int row) {
        return data[row];
    }
    
    /*!
     * @internal
     * @brief       Returns a given row from the matrix.
     * @param       row         Matrix's row to return.
     * @return      Pointer to the row's first index. Will be invalidated if the matrix is
     *              deleted from memory.
     */
    const long double* const
    operator[] (int row) const {
        return data[row];
    }
    
    /*!
     * @internal
     * @brief       Prints a matrix to an output stream.
     */
    friend std::ostream& operator<<(std::ostream& s, const matrix& m) {
        for(int i = 0; i < rows; ++i) {
            for(int j = 0; j < cols; ++j) {
                s << m[i][j] << "\t";
            }
            s << "\n";
        }
        return s;
    }
    
    long double       data[rows][cols];
    
};


// MARK: -
// MARK: Compound operators

// MARK: Matrix-to-Matrix ops

/*!
 * @internal
 * @brief       Add a matrix to <code>this</code>
 */
template<int r, int c> matrix<r,c>&
operator+=(matrix<r,c>& lhs, const matrix<r,c>& rhs) {
    for(int i = 0; i < r; ++i) {
        for(int j = 0; j < c; ++j) {
            lhs.data[i][j] += rhs.data[i][j];
        }
    }
    return lhs;
}

/*!
 * @internal
 * @brief       Subtract a matrix from <code>this</code>
 */
template<int r, int c> matrix<r,c>&
operator-=(matrix<r,c>& lhs, const matrix<r,c>& rhs) {
    for(int i = 0; i < r; ++i) {
        for(int j = 0; j < c; ++j) {
            lhs.data[i][j] -= rhs.data[i][j];
        }
    }
    return lhs;
}

// MARK: Matrix-to-scalar ops

/*!
 * @internal
 * @brief       Multiply <code>this</code> by a scalar.
 */
template<int r, int c> matrix<r,c>&
operator*=(matrix<r,c>& lhs, long double scalar) {
    for(int i = 0; i < r; ++i) {
        for(int j = 0; j < c; ++j) {
            lhs.data[i][j] *= scalar;
        }
    }
    return lhs;
}

/*!
 * @internal
 * @brief       Divide <code>this</code> by a scalar.
 */
template<int r, int c> matrix<r,c>&
operator/=(matrix<r,c>& lhs, long double scalar) {
    for(int i = 0; i < r; ++i) {
        for(int j = 0; j < c; ++j) {
            lhs.data[i][j] /= scalar;
        }
    }
    return lhs;
}

// MARK: -
// MARK: Binary operators

// MARK: Matrix-to-Matrix ops

/*!
 * @internal
 * @brief       Add two matrices together.
 */
template<int r, int c> const matrix<r,c>
operator+(const matrix<r,c>& lhs, const matrix<r,c>& rhs) {
    matrix<r,c> m(lhs);
    return m += rhs;
}

/*!
 * @brief       Subtract a matrix from another.
 */
template<int r, int c> const matrix<r,c>
operator-(const matrix<r,c>& lhs, const matrix<r,c>& rhs) {
    matrix<r,c> m(lhs);
    return m -= rhs;
}

/*!
 * @internal
 * @brief       Multiply a matrix by another.
 */
template<int a, int b, int c> const matrix<a, c>
operator*(const matrix<a,b>& lhs, const matrix<b,c>& rhs) {
    //auto _rhs = rhs.transpose();
    matrix<a,c> p = 0;
    long double temp;
    for(int i = 0; i < a; ++i) {
        for(int j = 0; j < c; ++j) {
            temp = 0;
            for(int k = 0; k < b; ++k) {
                temp += lhs[i][k] * rhs[k][j];
            }
            p[i][j] = temp;
        }
    }
    return p;
}

// MARK: Matrix-to-scalar ops



/*!
 * @internal
 * @brief       Multiply a matrix by a scalar.
 */
template<int r, int c> const matrix<r,c>
operator*(const matrix<r,c>& lhs, long double rhs) {
    matrix<r,c> m(lhs);
    return m *= rhs;
}

/*!
 * @internal
 * @brief       Multiply a matrix by a scalar.
 */
template<int r, int c> const matrix<r,c>
operator*(long double lhs, const matrix<r,c>& rhs) {
    matrix<r,c> m(rhs);
    return m *= lhs;
}

/*!
 * @brief       Divide a matrix by a scalar.
 */
template<int r, int c> const matrix<r,c>
operator/(const matrix<r,c>& lhs, long double rhs) {
    matrix<r,c> m(lhs);
    return m /= rhs;
}

/*!
 * @internal
 * @brief       3*3 square matrix.
 */
typedef matrix<3,3> Mat33;
/*!
 * @internal
 * @brief       4*4 square matrix.
 */
typedef matrix<4,4> Mat44;

