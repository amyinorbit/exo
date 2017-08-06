//
//  main.cpp
//  trappist
//
//  Created by Amy Parent on 06/03/2017.
//  Copyright © 2017 Amy Parent. All rights reserved.
//

#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <clocale>
#include <getopt.h>
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

void printUsage(const char* calledName) {
    std::cerr << "usage: " << calledName << " [-w width] [-h height] [-f] [-s step] json_file " << std::endl;
    std::cerr << std::endl;
    std::cerr << "\t-w,--width:\twindow width (defaults to 800 pixels)" << std::endl;
    std::cerr << "\t-h,--height:\twindow height (defaults to 600 pixels)" << std::endl;
    std::cerr << "\t-s,--step:\ttime increment between integration steps (defaults to 60 seconds)" << std::endl;
    std::cerr << "\tjson_file:\tjson solar system file" << std::endl;
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

// Mark: - Program entry point. Should porbably move to an App class

int main(int argc, char** args) {

    std::setlocale(LC_ALL, "");
    int             fullscreen      = 0;
    uint32_t        width           = 800;
    uint32_t        height          = 600;
    double          timestep        = 60.0;
    const char*     jsonpath        = nullptr;
    
    static struct option options[] =
    {
        {"width",       required_argument,  nullptr,        'w'},
        {"height",      required_argument,  nullptr,        'h'},
        {"step",    required_argument,  nullptr,            's'},
        {"fullscreen",  no_argument,        &fullscreen,     1 },
        {NULL, 0, NULL, 0}
    };
    
    int c = -1;
    while((c = getopt_long(argc, args, "w:h:s:f", options, NULL)) != -1) {
        switch(c) {
            case 'w':
                width = std::atoi(optarg);
                break;
            case 'h':
                height = std::atoi(optarg);
                break;
            case 's':
                timestep = std::atof(optarg);
                break;
            case 'f':
                fullscreen = 1;
                break;
            case '?':
                printUsage(args[0]);
                std::exit(EXIT_FAILURE);
                break;
            default:
                break;
        }
    }
    
    if(argc - optind != 1) {
        printUsage(args[0]);
        std::exit(EXIT_FAILURE);
    }
    jsonpath = args[optind];
    
    std::ifstream in{jsonpath};
    
    if(!in.is_open()) {
        std::cerr << "error: cannot open '" << jsonpath << "' for reading" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    
    time_t seconds = time(nullptr);
    StarSystem system{in, Physics::julianFromUnix(seconds)};
    in.close();
    
    Renderer renderer{width, height, jsonpath, static_cast<bool>(fullscreen)};
    
    renderer.onMouseDrag = &onMouseDrag;
    renderer.onMouseScroll = &onMouseScroll;
    renderer.onKeyDown = &onKeyDown;
    
    renderer.setScale(100.0 / system.maxDiameter());
    renderer.start([&](Renderer& renderer) {
        
        seconds += system.advance(iterations, timestep);
        system.render(renderer);
        renderer.setColor(Renderer::Color::WHITE);
        renderer.drawUIString(Vector3{-0.47, 0.47, 0}, dateString(seconds));
        renderer.drawUIString(Vector3{-0.47, -0.47, 0}, std::to_string(iterations) + " steps/frame");
        
        return true;
    });
    
    return 0;
}
