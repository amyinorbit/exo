//
//  Vector3.hpp
//  engine3
//
//  Created by Cesar Parent on 29/03/2016.
//  Copyright © 2016 cesarparent. All rights reserved.
//

#pragma once
#include <iostream>
#include <cstdint>
#include <cstddef>
#include <cmath>
#include "matrix.hpp"


/**
 * vec3<T> represents vectors, size or points in 2D space.
 * @tparam T    The vector's components type.
 */
template <typename T>
struct vec3 final {
    
    typedef T CompType;
    
    /**
     * Creates a zero vector.
     */
    explicit vec3() : x(0), y(0), z(0) {}
    
    /**
     * Creates a vector, given two co-ordinates.
     * @param xx    The x co-ordinate.
     * @param yy    The y co-ordinate.
     * @param zz    The z co-ordinate.
     */
    explicit vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
    
    /**
     * Creates a vector by copying another one./
     * @param rhs   The vector to copy.
     */
    vec3(const vec3& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}
    
    /**
     * Assigns another vector's co-ordinates to the vector.
     * @param rhs   The vector to assign to this.
     */
    vec3& operator=(const vec3& rhs) {
        if(this != &rhs) {
            x = rhs.x;
            y = rhs.y;
            z = rhs.z;
        }
        return *this;
    }
    
    /**
     * Casts the vector to another 2D vector type.
     */
    template <typename T2>
    operator vec3<T2>() {
        return vec3<T2>{static_cast<T2>(x), static_cast<T2>(y), static_cast<T2>(z)};
    }
    
#pragma mark vec3 properties
    
    /**
     * Returns one of the vector's component by index.
     * @param index     The component to return (0 for x, 1 for y, 2 for z).
     * @return A reference to the component.
     */
    T& operator[](std::size_t index) {
        return data[index];
    }
    
    /**
     * Returns one of the vector's components value by index.
     * @param index     The component which value to return (0 for x, 1 for y, 2 for z).
     * @return The value to the component.
     */
    T operator[](std::size_t index) const {
        return data[index];
    }
    
    /**
     * Returns the vector's magnitude.
     * @return The vector's magnitude.
     */
    T magnitude() const {
        return std::sqrt(x*x + y*y + z*z);
    }
    
    /**
     * Returns the vector, with a magnitude of 1.
     * @return The vector, with a magnitude of 1.
     */
    vec3 normalized(double length = 1.0) const {
        return length * (vec3(*this) / magnitude());
    }
    
#pragma mark static vec3 API
    
    /**
     * Scales a vector to a magnitude of 1.
     * @param vector    The vector to scale.
     */
    static void normalize(vec3& vector) {
        vector /= vector.magnitude();
    }
    
    /**
     * Returns the dot product of two vectors.
     * @param a     The first member of the dot product.
     * @param b     The second member of the dot product.
     * @return The dot product a.b
     */
    static T dot(const vec3& a, const vec3& b) {
        return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
    }
    
    /**
     * Returns the distance between two points.
     * @param a     The co-ordinates of the first point.
     * @param b     The co-ordinates of the second point.
     * @return The distance between a and b.
     */
    static T distance(const vec3& a, const vec3& b) {
        return (a - b).magnitude();
    }
    
    /**
     * Returns the linear interpolation of two vectors by a scalar.
     * The result is a for t=0, and b for t=1.
     * @param a     The vector to interpolate from.
     * @param b     The vector to interpolate to.
     * @param t     The scalar to interpolate by. t is clamped in [0, 1].
     * @return The linear interpolation between a and b by t.
     */
    static vec3 lerp(const vec3& a, const vec3& b, float t) {
        if(t == 0) return a;
        if(t == 1 || a == b) return b;
        if(t > 1) t = 1;
        if(t < 0) t = 0;
        return (1-t)*a + t*b;
    }
    
    static vec3 slerp(const vec3& a, const vec3& b, float t) {
        if(t == 0) return a;
        if(t == 1 || a == b) return b;
        
        float w = std::acos(Dot(a, b));
        float sw = std::sin(w);
        return (std::sin((1-t)*w)/sw)*a + (std::sin(t*w)/sw)*b;
    }
    
    /*!
     * Rodriges rotation formula
     */
    vec3 rotate(const vec3& axis, double angle) const {
        auto p = normalized();
        auto a = axis.normalized();
        auto s = std::sin(angle);
        auto c = std::cos(angle);
        
        auto t = Mat33{
            a.x*a.x*(1-c) + c,      a.x*a.y*(1-c) - a.z*s,  a.x*a.z*(1-c) + a.y*s,
            a.y*a.x*(1-c) + a.z*s,  a.y*a.y*(1-c) + c,      a.y*a.z*(1-c) - a.x*s,
            a.z*a.x*(1-c) - a.y*s,  a.z*a.y*(1-c) + a.x*s,  a.z*a.z*(1-c) + c,
        };
        
        return t * p;
    }
    
    static vec3 cross(const vec3& a, const vec3& b) {
        return vec3 {
             (a.y*b.z - a.z*b.y),
            -(a.x*b.z - a.z*b.x),
             (a.x*b.y - a.y*b.x)
        };
    }
    
#pragma vec3 data
    
    union {
        /// The vector's members.
        T data[3];
        struct {
            /// The vector's x co-ordinate.
            T x;
            /// The vector's y co-ordinate.
            T y;
            /// The vector's z co-ordinate.
            T z;
        };
    };
};

template <typename T>
std::ostream& operator<<(std::ostream& lhs, const vec3<T>& rhs) {
    lhs << "{" << rhs.x << ", " << rhs.y << ", " << rhs.z << "}";
    return lhs;
}

/**
 * Returns whether two vectors are equal.
 * @param a     The first vector.
 * @param b     The second vector.
 * @return Whether a and b are equals.
 */
template <typename T>
bool operator==(const vec3<T>& a, const vec3<T>& b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

/**
 * Returns whether two vectors are different.
 * @param a     The first vector.
 * @param b     The second vector.
 * @return Whether a and b are different.
 */
template <typename T>
bool operator!=(const vec3<T>& a, const vec3<T>& b) {
    return !(a == b);
}

#pragma mark Compound operators

/**
 * Returns the opposite of a vector.
 * @param lhs   The first vector.
 * @return The opposite of a vector.
 */
template <typename T>
vec3<T>& operator-(vec3<T>& lhs) {
    lhs.x = -lhs.x;
    lhs.y = -lhs.y;
    lhs.z = -lhs.z;
    return lhs;
}

/**
 * Adds a vector to another.
 * @param lhs   The vector to add to.
 * @param rhs   The vector to add.
 * @return A reference to the first vector.
 */
template <typename T>
vec3<T>& operator+=(vec3<T>& lhs, const vec3<T>& rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    return lhs;
}

/**
 * Subtracts a vector from another.
 * @param lhs   The vector to subtract from.
 * @param rhs   The vector to subtract.
 * @return A reference to the first vector.
 */
template <typename T>
vec3<T>& operator-=(vec3<T>& lhs, const vec3<T>& rhs) {
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
    return lhs;
}

/**
 * Divides a vector's components by another's.
 * @param lhs   The vector to divide.
 * @param rhs   The vector to divide by.
 * @return A reference to the first vector.
 */
template <typename T>
vec3<T>& operator/=(vec3<T>& lhs, const vec3<T>& rhs) {
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
    lhs.z /= rhs.z;
    return lhs;
}

/**
 * Divides a vector's components by a scalar.
 * @param lhs   The vector to divide.
 * @param rhs   The scalar to divide by.
 * @return A reference to the first vector.
 */
template <typename T>
vec3<T>& operator/=(vec3<T>& lhs, float rhs) {
    lhs.x /= rhs;
    lhs.y /= rhs;
    lhs.z /= rhs;
    return lhs;
}

/**
 * Multiplies a vector's components by another's.
 * @param lhs   The vector to divide.
 * @param rhs   The vector to divide by.
 * @return A reference to the first vector.
 */
template <typename T>
vec3<T>& operator*=(vec3<T>& lhs, const vec3<T>& rhs) {
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;
    lhs.z *= rhs.z;
    return lhs;
}

/**
 * Multiplies a vector's components by a scalar.
 * @param lhs   The vector to multiply.
 * @param rhs   The scalar to multiply by.
 * @return A reference to the first vector.
 */
template <typename T>
vec3<T>& operator*=(vec3<T>& lhs, float rhs) {
    lhs.x *= rhs;
    lhs.y *= rhs;
    lhs.z *= rhs;
    return lhs;
}

#pragma mark Binary Operators

/**
 * Adds to vectors together and returns the result.
 * @param lhs   The first vector.
 * @param rhs   The second vector.
 * @return The result of the addition.
 */
template <typename T>
const vec3<T> operator+(const vec3<T>& lhs, const vec3<T>& rhs) {
    vec3<T> result{lhs};
    return result += rhs;
}

/**
 * Returns the opposite of a vector
 * @param lhs   The first vector.
 * @return The result of the subtraction.
 */
template <typename T>
const vec3<T> operator-(const vec3<T>& lhs) {
    return -1.0 * lhs;
}

/**
 * Subtract a vector from another and returns the result.
 * @param lhs   The first vector.
 * @param rhs   The second vector.
 * @return The result of the subtraction.
 */
template <typename T>
const vec3<T> operator-(const vec3<T>& lhs, const vec3<T>& rhs) {
    vec3<T> result{lhs};
    return result -= rhs;
}

/**
 * Divides a vector by another member-wise and returns the result.
 * @param lhs   The first vector.
 * @param rhs   The second vector.
 * @return The result of the division.
 */
template <typename T>
const vec3<T> operator/(const vec3<T>& lhs, const vec3<T>& rhs) {
    vec3<T> result{lhs};
    return result /= rhs;
}

/**
 * Divides a vector by a scalar and returns the result.
 * @param lhs   The vector.
 * @param rhs   The scalar.
 * @return The result of the division.
 */
template <typename T>
const vec3<T> operator/(const vec3<T>& lhs, float rhs) {
    vec3<T> result{lhs};
    return result /= rhs;
}

/**
 * Multiplies a vector by another member-wise and returns the result.
 * @param lhs   The first vector.
 * @param rhs   The second vector.
 * @return The result of the product.
 */
template <typename T>
const vec3<T> operator*(const vec3<T>& lhs, const vec3<T>& rhs) {
    vec3<T> result{lhs};
    return result *= rhs;
}

/**
 * Multiplies a vector by a scalar and returns the result.
 * @param lhs   The vector.
 * @param rhs   The scalar.
 * @return The result of the product.
 */
template <typename T>
const vec3<T> operator*(const vec3<T>& lhs, float rhs) {
    vec3<T> result{lhs};
    return result *= rhs;
}

/**
 * Multiplies a vector by a scalar and returns the result.
 * @param lhs   The scalar.
 * @param rhs   The vector.
 * @return The result of the product.
 */
template <typename T>
const vec3<T> operator*(float lhs, const vec3<T>& rhs) {
    vec3<T> result{rhs};
    return result *= lhs;
}

/*!
 * @internal
 * @brief       Multiply a vector by a matrix.
 */
template<typename T, int r>
const vec3<T> operator*(const matrix<r,3>& lhs, const vec3<T>& rhs) {
    vec3<T> v = vec3<T>{0, 0, 0};
    T temp;
    for(int i = 0; i < 3; ++i) {
        temp = 0;
        for(int k = 0; k < 3; ++k) {
            temp += (T)lhs[i][k] * rhs.data[k];
        }
        v.data[i] = temp;
    }
    return v;
}

/// The most used vector type
typedef vec3<long double> Vector3;
