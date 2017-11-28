
#pragma once
#include "Utils.hpp"
#include "matrix.hpp"
#include "vec3.hpp"

namespace Transform {
    
    static inline Mat44 projection(double fov, double a, double n, double f) {
        double d = f - n;
        double e = -1.0 / std::tan(0.5 * radians(fov));
        
        return Mat44 {
            e/a,    0,      0,      0,
            0,      e,      0,      0,
            0,      0,      -(f+n)/d,   -1,
            0,      0,      -(2*f*n)/d,  0,
        };
        //
        // proj[1][1] = -1.0 / std::tan(0.5 * radians(fov));
        // proj[0][0] = proj[1][1] / aspect;
        // proj[2][2] = far * oneOverDepth;
        // proj[3][2] = (-far * near) * oneOverDepth;
        // proj[2][3] = -1;
        // proj[3][3] = 0;
        //
        // return -1*proj;
    }
    
    static inline Mat44 rotateX(double angle) {
        double c = std::cos(angle);
        double s = std::sin(angle);
        
        return Mat44 {
            1,  0,  0,  0,
            0,  c, -s,  0,
            0,  s,  c,  0,
            0,  0,  0,  1
        };
    }
    
    static inline Mat44 rotateY(double angle) {
        double c = std::cos(angle);
        double s = std::sin(angle);
        
        return Mat44 {
            c,  0,  s,  0,
            0,  1,  0,  0,
           -s,  0,  c,  0,
            0,  0,  0,  1
        };
    }
    
    static inline Mat44 rotateZ(double angle) {
        double c = std::cos(angle);
        double s = std::sin(angle);
        
        return Mat44 {
            c, -s,  0,  0,
            s,  c,  0,  0,
            0,  0,  1,  0,
            0,  0,  0,  1
        };
    }
    
    static inline Mat44 translate(double x, double y, double z) {
        return Mat44 {
            1,  0,  0,  x,
            0,  1,  0,  y,
            0,  0,  1,  z,
            0,  0,  0,  1
        };
    }
    
    static inline Mat44 scale(double sx, double sy, double sz) {
        return Mat44 {
            sx, 0,  0,  0,
            0,  sy, 0,  0,
            0,  0,  sz, 0,
            0,  0,  0,  1
        };
    }
    
    static inline Mat44 scale(double s) {
        return scale(s, s, s);
    }
    
    static inline Vector3 apply(const Mat44& T, const Vector3& v) {
        matrix<4,1> vv {
            v.x,
            v.y,
            v.z,
            1.0
        };
        auto r = T * vv;
        return Vector3{r[0][0], r[1][0], r[2][0]};
    }
}



