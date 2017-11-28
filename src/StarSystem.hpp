//
//  StarSystem.hpp
//  trappist
//
//  Created by Amy Parent on 06/03/2017.
//  Copyright © 2017 Amy Parent. All rights reserved.
//
#pragma once
#include <deque>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include "Math/vec3.hpp"
#include "Renderer.hpp"
#include "Integrator.hpp"
#include "Orbit.hpp"

class StarSystem {
public:
    
    struct Body {
        std::string         name;
        Renderer::Color     color;
        Integrator::State   state;
        long double         mass;
        long double         radius;
        std::deque<Vector3> trail;
    };
    
    StarSystem(std::istream& jsonFile, long double julianDate);
    
    ~StarSystem() {}
    
    double maxDiameter();
    
    double advance(int iterations, double delta);
    
    void render(Renderer& renderer);
    
    const Body* nextBody() const;
    
    const std::vector<Body>& bodies() const { return bodies_; }
    
private:
    Vector3 accelerate(const Integrator::State& state, double mass);
    
    std::string         integrating_;
    int                 ticksToTrail_;
    
    mutable uint64_t    nextBody_;
    std::vector<Body>   bodies_;
    std::vector<Body>   previous_;
    
};
