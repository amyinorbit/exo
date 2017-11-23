//
//  Orbit.hpp
//  trappist
//
//  Created by Amy Parent on 08/03/2017.
//  Copyright © 2017 Amy Parent. All rights reserved.
//
#pragma once
#include <utility>
#include "Physics.hpp"
#include "Math/Utils.hpp"
#include "Math/vec3.hpp"

class Orbit {
    
    long double a_, e_, i_, arg_, raan_, m_, T_;
    
    Orbit(long double a, long double e, long double i,
          long double arg, long double raan, long double m, long double T)
        : a_(a), e_(e), i_(i), arg_(arg), raan_(raan), m_(m), T_(T) {}
            
    std::pair<Vector3, Vector3> hyperbolicSV(long double GM, long double t) const {
        
        long double n = std::sqrt(GM/(-a_*-a_*-a_));
        long double M = m_ + n * ((t - T_) * 86400.0);
        
        long double H = M;
        long double delta = 1.0;
        long double epsilon = 1e-10;
        
        while(delta > epsilon) {
            long double newH = H + (M - (e_ * std::sinh(H)) + H)/((e_ * std::cosh(H)) - 1);
            delta = std::abs(H - newH);
            H = newH;
        }
        
        // true anomaly:
        long double v = 2 * std::atan(std::sqrt((e_+1.0)/(e_-1.0)) * std::tanh(H/2.0));
        long double r = (a_ * (1.0 - (e_ * e_))) / (1.0 + e_ * std::cos(v));
        long double h = std::sqrt(GM * a_ * (1.0 - (e_ * e_)));
        long double p = a_ * (1.0 - (e_ * e_));
        
        Vector3 pos{
            r * (std::cos(raan_)*std::cos(arg_ + v) - std::sin(raan_)*std::sin(arg_ + v) * std::cos(i_)),
            r * (std::sin(raan_)*std::cos(arg_ + v) + std::cos(raan_)*std::sin(arg_ + v) * std::cos(i_)),
            r * (std::sin(i_)*std::sin(arg_ + v)),
        };
        
        
        Vector3 vel{
            ((pos.x*h*e_) / (r*p)) * std::sin(v) - (h/r) * (std::cos(raan_) * std::sin(arg_ + v)
                                                            + std::sin(raan_) * std::cos(arg_ + v) * std::cos(i_)),
            ((pos.y*h*e_) / (r*p)) * std::sin(v) - (h/r) * (std::sin(raan_) * std::sin(arg_ + v)
                                                            - std::cos(raan_) * std::cos(arg_ + v) * std::cos(i_)),
            ((pos.z*h*e_) / (r*p)) * std::sin(v) + (h/r) * (std::cos(arg_ + v) * std::sin(i_))
        };
        
        return std::make_pair(pos, vel);
    }
    
public:
    
    std::pair<Vector3, Vector3> stateVectors(long double GM,
                                             long double t = Physics::J2000) const {
        
        // eccentric anomaly, using kepler's method
        // http://www.bogan.ca/orbits/kepler/keplerex.html
        if(e_ > 1.0 || a_ <= 0) { return hyperbolicSV(GM, t); }
        
        long double a = std::abs(a_);
        long double n = std::sqrt(GM/(a*a*a));
        long double Ma = m_ + n * ((t - T_) * 86400.0);
        
        long double Ea = Ma + e_ * std::sin(Ma);
        long double delta = 1.0;
        long double epsilon = 1e-10;
        
        while(delta > epsilon) {
            long double newEa = Ma + e_ * std::sin(Ea);
            delta = std::abs(Ea - newEa);
            Ea = newEa;
        }
        
        // true anomaly:
        long double v = 2 * std::atan(std::sqrt((1.0+e_)/(1.0-e_)) * std::tan(Ea/2.0));
        long double r = (a * (1.0 - (e_ * e_))) / (1.0 + e_ * std::cos(v));
        long double h = std::sqrt(GM * a * (1.0 - (e_ * e_)));
        
        long double p = a * (1.0 - (e_ * e_));
        
        Vector3 pos{
            r * (std::cos(raan_)*std::cos(arg_ + v) - std::sin(raan_)*std::sin(arg_ + v) * std::cos(i_)),
            r * (std::sin(raan_)*std::cos(arg_ + v) + std::cos(raan_)*std::sin(arg_ + v) * std::cos(i_)),
            r * (std::sin(i_)*std::sin(arg_ + v)),
        };
        
        
        Vector3 vel{
            ((pos.x*h*e_) / (r*p)) * std::sin(v) - (h/r) * (std::cos(raan_) * std::sin(arg_ + v)
                                                            + std::sin(raan_) * std::cos(arg_ + v) * std::cos(i_)),
            ((pos.y*h*e_) / (r*p)) * std::sin(v) - (h/r) * (std::sin(raan_) * std::sin(arg_ + v)
                                                            - std::cos(raan_) * std::cos(arg_ + v) * std::cos(i_)),
            ((pos.z*h*e_) / (r*p)) * std::sin(v) + (h/r) * (std::cos(arg_ + v) * std::sin(i_))
        };
        
        return std::make_pair(pos, vel);
    }
    
    // MARK: - orbit builder. Might VERY WELL go away
    
    class Builder {
        
        long double a_, e_, i_, arg_, raan_, m_, T_;
        
    public:
        
        Builder() : a_(0), e_(0), i_(0), arg_(0), raan_(0), m_(0), T_(Physics::J2000) {}
        
        Builder& semiMajorAxis(long double a) { a_ = a; return *this; }
        Builder& eccentricity(long double e) { e_ = e; return *this; }
        Builder& inclination(long double i) { i_ = i; return *this; }
        Builder& argOfPeriapsis(long double arg) { arg_ = arg; return *this; }
        Builder& rightAscension(long double raan) { raan_ = raan; return *this; }
        Builder& meanAnomaly(long double m) { m_ = m; return *this; }
        Builder& epoch(long double T) { T_ = T; return *this; }
        
        Orbit build() {
            return Orbit(a_, e_, radians(i_), radians(arg_), radians(raan_), radians(m_), T_);
        }
    };
    
    friend std::ostream& operator<<(std::ostream& out, const Orbit& orbit);
    
};

inline std::ostream& operator<<(std::ostream& out, const Orbit& orbit) {
    out << "ORB(";
    out << "SmA:"<<orbit.a_<<",";
    out << "Ecc:"<<orbit.e_<<",";
    out << "Inc:"<<orbit.i_<<",";
    out << "RAN:"<<orbit.raan_<<",";
    out << "Arg:"<<orbit.arg_<<",";
    out << "MAn:"<<orbit.m_;
    out << ")";
    return out;
}
