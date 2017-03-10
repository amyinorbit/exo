//
// HADS
// Integrator.cpp - Physics integrator implementation.
//
// For simplicity's sake, the integrator uses Euler's method for the time
// being. Ideally, it will be swapped for the Runge-Kutta 4 method, but there
// shouldn't be that bgi a difference in precision for our application.
//
// authors:     Cesar Parent <cesar@cesarparent.com>
//
#include "Integrator.hpp"

namespace Integrator {
    
    State advance(const State& state, long double mass, Accelerator callback, double dt) {
        
        // Verlet Integrator:
        //    float halfdt = 0.5f * dt;
        //    float invmass = 1.0f / mass;
        //    ...
        //    do
        //        ...
        //        r += v * dt + a * halfdt * dt;
        //        v += a * halfdt;
        //        a = f * invmass;
        //        v += a * halfdt;
        //        ...
        //        loop
        
        float halfdt = 0.5 * dt;
        
        State next = state;
        
        next.position += state.velocity * dt + state.acceleration * halfdt * dt;
        next.velocity += state.acceleration * halfdt;
        next.acceleration = callback(next, mass);
        next.velocity += next.acceleration * halfdt;
        return next;
    }
}
