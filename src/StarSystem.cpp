//
//  StarSystem.cpp
//  trappist
//
//  Created by Amy Parent on 06/03/2017.
//  Copyright © 2017 Amy Parent. All rights reserved.
//
#include <functional>
#include <cstdlib>
#include <algorithm>
#include "json.hpp"
#include "StarSystem.hpp"
#include "Physics.hpp"
#include "Orbit.hpp"
#include "Model.hpp"

using json = nlohmann::json;

static const int TRAIL_SIZE = 80;
static const int TRAIL_TICK = 100;

template <typename T>
T get(const json& data, const std::string& key, T fallback) {
    if(data.count(key) > 0) {
        return data[key].get<T>();
    }
    return fallback;
}

StarSystem::StarSystem(std::istream& jsonFile, long double julianDate) {
    
    nextBody_ = 1;
    ticksToTrail_ = 0;
    json data;
    jsonFile >> data;
    
    if(data.count("star") != 1) {
        std::cerr << "error: No star entry in solar system file" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    if(data.count("bodies") != 1) {
        std::cerr << "error: No stars entry in solar system file" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    
    auto& star = data["star"];
    
    Integrator::State state {
        Vector3{0.0, 0.0, 0.0},
        Vector3{-20.0, 0.0, 0.0},
        Vector3{0.0, 0.0, 0.0}
    };
    
    bodies_.push_back(Body{
        get<std::string>(star, "name", "SYSTEM a"),
        Renderer::Color::YELLOW,
        state,
        get(star, "mass", 1.0) * Physics::Msol,
        get(star, "radius", 1.0) * Physics::Rsol
    });
    
    
    // Keep track of the raw orbits, we can only get valid state vectors once
    // we know where the barycenter of the whole system is
    
    std::vector<Orbit> orbits_;
    
    for(auto& body : data["bodies"]) {
        char ID = 'b';
        
        auto color = Renderer::Color::LIGHTBLUE;
        if(body.count("color") > 0) {
            color = Renderer::colorNamed(body["color"].get<std::string>());
        }
        
        long double mass = get(body, "mass", 1.0) * Physics::Mearth;
        auto orbit = Orbit::Builder()
            .semiMajorAxis(get(body, "sma", 1.0) * Physics::AU)
            .eccentricity(get(body, "ecc", 0.0))
            .inclination(get(body, "inc", 0.0))
            .argOfPeriapsis(get(body, "arg", 0.0))
            .rightAscension(get(body, "raan", 0.0))
            .meanAnomaly(get(body, "ma", 0.0))
            .epoch(get(body, "epoch", Physics::J2000))
            .build();
        //std::cout << orbit << std::endl;
        
        // First estimation, we only use that to get the position - velocity
        // will not be accurate until we have a complete system mass and
        // barycenter location.
        auto stateVectors = orbit.stateVectors((bodies_[0].mass + mass) * Physics::G, julianDate);
        //std::cout << "\t-> " << stateVectors.first << ", " << stateVectors.second << std::endl;
        
        Integrator::State state {
            stateVectors.first,
            stateVectors.second,
            Vector3{}
        };
        
        bodies_.push_back(Body{
            get<std::string>(body, "name", "SYSTEM" + std::to_string(ID++)),
            color,
            state,
            mass,
            get(body, "radius", 1.0) * Physics::Rearth
        });
        // const auto& B = bodies_.back();
        // std::cout << "Found " << B.name << " with radius: " << B.radius / Physics::Rearth << std::endl;
        orbits_.push_back(orbit);
    }
    
    Vector3 barycenter{};
    Vector3 momentum{};
    long double mass = 0;
    
    for(auto& body : bodies_) {
        barycenter  += body.mass * body.state.position;
        mass        += body.mass;
    }
    
    barycenter /= mass;
    
    for(size_t i = 1; i < bodies_.size(); ++i) {
        auto& body = bodies_[i];
        auto& orbit = orbits_[i-1];
        
        auto stateVectors = orbit.stateVectors(mass * Physics::G, julianDate);
        body.state.position = stateVectors.first;
        body.state.velocity = stateVectors.second;
    }
    
    for(auto& body : bodies_) {
        momentum += body.mass * body.state.velocity;
    }
    momentum /= mass;

    for(auto& body: bodies_) {
        body.state.position -= barycenter;
        body.state.velocity -= momentum;
    }
}


const StarSystem::Body* StarSystem::nextBody() const {
    if(bodies_.size() <= 0) { return NULL; }
    const Body* b = &bodies_[nextBody_];
    nextBody_ = (nextBody_ + 1) % bodies_.size();
    return b;
}

double StarSystem::maxDiameter() {
    double radius = 0;
    
    for(auto& body : bodies_) {
        auto sma = body.state.position.magnitude();
        if(sma <= 0) { continue; }
        if(sma > radius) {
            radius = sma;
        }
    }
    return radius * 2;
    //return bodies_[0].state.position.magnitude() * 2;
}

Vector3 StarSystem::accelerate(const Integrator::State& state, double mass) {
    Vector3 forces;
    
    for(auto& body: previous_) {
        if(body.name == integrating_) { continue; }
        
        auto ray = (body.state.position - state.position).normalized();
        forces += ray * Physics::gravity(state.position, mass, body.state.position, body.mass);
    }
    return forces / mass;
}

using namespace std::placeholders;

double StarSystem::advance(int iterations, double delta) {
    
    for(int i = 0; i < iterations; ++i) {
        previous_ = bodies_;
        for(auto& body: bodies_) {
            integrating_ = body.name;
            body.state = Integrator::advance(body.state,
                                             body.mass,
                                             std::bind(&StarSystem::accelerate, this, _1, _2),
                                             delta);
        }
        if(ticksToTrail_-- == 0) {
            ticksToTrail_ = TRAIL_TICK;
            
            for(auto& body: bodies_) {
                body.trail.push_front(body.state.position);
                if(body.trail.size() > TRAIL_SIZE) {
                    body.trail.pop_back();
                }
            }
        }
    }
    
    return delta * iterations;
}

void StarSystem::render(Renderer &renderer) {
    for(auto& body: bodies_) {
        renderer.setColor(body.color);
        renderer.drawModel(Model::sphereInstance(), body.state.position, 10*body.radius);
        //renderer.drawCircle(body.state.position, 4);
        renderer.drawString(body.state.position + Vector3{0, 0, 10*body.radius}, body.name);
        Vector3 previous = body.state.position;
        
        int it = TRAIL_SIZE-1;
        for(auto& p : body.trail) {
            Vector3 vert = p;
            
            double alpha = (double)it / (double)TRAIL_SIZE;
            renderer.setColor(body.color, alpha);
            it--;
            renderer.drawLine(previous, vert);
            previous = vert;
        }
    }
}

