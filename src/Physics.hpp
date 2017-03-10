//
//  physics.hpp
//  trappist
//
//  Created by Amy Parent on 06/03/2017.
//  Copyright © 2017 cesarparent. All rights reserved.
//
#pragma once
#include <ctime>
#include "Math/vec3.hpp"
#include "Integrator.hpp"

namespace Physics {
    
    const long double Msol = 1.98855e30;
    const long double Mearth = 5.9722e24;
    const long double AU = 149597870700;
    const long double G = 6.674e-11;
    
    const long double Minute = 60;
    const long double Hour = 3600;
    const long double Day = 23 * Hour + 56 * Minute + 4.1;
    
    const long double J2000 = 2451545.0;
    
    static inline uint64_t unixFromJulian(long double jd) {
        return (jd - 2440587.5) * 86400.0;
    }
    
    static inline long double julianFromUnix(uint64_t unix) {
        return (unix / 86400.0) + 2440587.5;
    }
    
    long double gravity(const Vector3& body1, long double mass1,
                        const Vector3& body2, long double mass2);
    
}
