//
// HADS
// Integrator.hpp - Physics integrator interface.
//
// authors:     Amy Parent <amy@amyparent.com>
//
#pragma once
#include <functional>
#include "Math/vec3.hpp"

namespace Integrator {
    
    // The state of a simulated body, at a given time.
    struct State {
        Vector3     position;
        Vector3     velocity;
        Vector3     acceleration;
    };
    
    // A function that returns the acceleration vector on a solid body.
    typedef std::function<Vector3(const State&, long double)> Accelerator;
    
    // Returns the state of a solid body time t+dt, based on the its state
    // at time t, the forces applied to it and the time increment dt.
    //
    // The Accelerator callback will be called during integration to obtain
    // the acceleration vector for a given state.
    State advance(const State& state, long double mass, Accelerator callback, double dt);

}
