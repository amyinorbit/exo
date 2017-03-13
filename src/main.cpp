//
//  main.cpp
//  trappist
//
//  Created by Amy Parent on 06/03/2017.
//  Copyright © 2017 cesarparent. All rights reserved.
//

#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <clocale>
#include "Math/Utils.hpp"
#include "Physics.hpp"
#include "Renderer.hpp"
#include "StarSystem.hpp"

static int iterations = 0;

std::string dateString(time_t seconds) {
    long double jd = Physics::julianFromUnix(seconds);
    std::tm *ltm = std::localtime(&seconds);
    
    char niceDate[256];
    
    std::strftime(niceDate, 256, "ET: %b %d, %Y", ltm);
    
    return "JD: " + std::to_string((uint64_t)jd) + ", " + niceDate;
}

void onKeyDown(Renderer& r, SDL_Scancode key) {
    switch(key) {
        case SDL_SCANCODE_LEFT:
            r.rotate(0, -2, 0);
            break;
        case SDL_SCANCODE_RIGHT:
            r.rotate(0, 2, 0);
            break;
        case SDL_SCANCODE_UP:
            iterations *= 2;
            break;
        case SDL_SCANCODE_DOWN:
            iterations /= 2;
            if(iterations < 1) { iterations = 1; }
            break;
        case SDL_SCANCODE_SPACE:
            iterations = 0;
            break;
        case SDL_SCANCODE_1:
        case SDL_SCANCODE_2:
        case SDL_SCANCODE_3:
        case SDL_SCANCODE_4:
        case SDL_SCANCODE_5:
        case SDL_SCANCODE_6:
        case SDL_SCANCODE_7:
        case SDL_SCANCODE_8:
        case SDL_SCANCODE_9:
        case SDL_SCANCODE_0:
            iterations = 10 * (1 + key - SDL_SCANCODE_1);
            break;
        default:
            break;
    }
}

void onMouseDrag(Renderer& r, double dx, double dy) {
    r.rotate(-300*dy, 0, 300*dx);
}

void onMouseScroll(Renderer& r, double dx, double dy) {
    r.zoom(-5*dy);
}

int main(int argc, char** args) {
    
    uint32_t width = 800, height = 600;
    double timestep = 60.0;
    std::setlocale(LC_ALL, "");
    
    if(argc != 3 && argc != 5) {
        std::cerr << "usage: " << args[0] << " infile step [width height]" << std::endl;
        std::cerr << "\tinfile:\tjson solar system file" << std::endl;
        std::cerr << "\tstep:\ttime increment between integration steps (seconds)" << std::endl;
        std::cerr << "\twidth:\twindow width (pixels)" << std::endl;
        std::cerr << "\theight:\twindow height (pixels)" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::ifstream in{args[1]};
    
    if(!in.is_open()) {
        std::cerr << "error: cannot open '" << args[1] << "' for reading" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    
    timestep = std::atof(args[2]);
    
    if(argc == 5) {
        width = std::atoi(args[3]);
        height = std::atoi(args[4]);
    }
    
    time_t      seconds = time(nullptr);
    StarSystem system{in, Physics::julianFromUnix(seconds)};
    in.close();
    
    Renderer renderer{width, height, args[1]};
    
    renderer.onMouseDrag = &onMouseDrag;
    renderer.onMouseScroll = &onMouseScroll;
    renderer.onKeyDown = &onKeyDown;
    
    renderer.setScale(100.0 / system.maxDiameter());
    renderer.start([&](Renderer& renderer) {
        
        seconds += system.advance(iterations, timestep);
        system.render(renderer);
        
        //renderer.setScale(1.0);
        renderer.setColor(Renderer::Color::WHITE);
        renderer.drawUIString(Vector3{-0.47, 0.47, 0}, dateString(seconds));
        renderer.drawUIString(Vector3{-0.47, -0.47, 0}, std::to_string(iterations) + " steps/frame");
        
        return true;
    });
    
    return 0;
}
