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
#include <algorithm>
#include <iterator>
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
    std::cerr << "\t-j,--start:\tJulian Date of the simulation's start (defaults to now" << std::endl;
    std::cerr << "\tjson_file:\tjson solar system file" << std::endl;
}

static int bodySwitcher = 1;
static bool showNames = false;
static float mult = 1;

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
        case SDL_SCANCODE_EQUALS:
            mult = 1;
            break;
        case SDL_SCANCODE_MINUS:
            mult = -1;
            break;
        case SDL_SCANCODE_RIGHTBRACKET:
            bodySwitcher = 1;
            break;
        case SDL_SCANCODE_LEFTBRACKET:
            bodySwitcher = -1;
            break;
        case SDL_SCANCODE_TAB:
            showNames = !showNames;
            break;
        default:
            break;
    }
}

void onMouseDrag(Renderer& r, double dx, double dy) {
    r.rotate(-300*dy, 0, 300*dx);
}

static float scrollSpeed = 0.f;

void onMouseScroll(Renderer& r, double dx, double dy) {
    scrollSpeed = - 6.f*dy;
}

void drawBodiyList(Renderer& renderer, const std::vector<std::string>& names, uint64_t selected) {
    double deltaY = 0.03 * (600.0 / double(renderer.height()));
    double deltaX = 0.02 * (800.0 / double(renderer.width()));
    double height = deltaY * float(names.size() + 2);
    double width = deltaX * 10;
    double startX = -0.48 + deltaX;
    double startY = -((0.03 * float(names.size()))/2.0);
    double boxY = -(deltaY) + startY;
    
    renderer.drawUIBox(Vector3{-0.48, boxY, 0}, Vector3{width, height, 0}, Renderer::Color::BLACK, Renderer::Color::WHITE);
    
    for(size_t i = 0; i < names.size(); ++i) {
        const auto& name = names[i];
        renderer.setColor(i == selected ? Renderer::Color::PASTEL_YELLOW : Renderer::Color::WHITE);
        renderer.drawUIString(Vector3{startX, startY + (deltaY * float(i)), 0}, name);
    }
}

uint64_t changeBody(const StarSystem& system, Renderer& renderer, uint64_t current, int offset) {
    const auto& bodies = system.bodies();
    if(bodies.size() == 0) { return -1; }
    
    int64_t target = (current + offset) % bodies.size();
    if(renderer.setCenter(&(bodies[target].state.position))) {
        return target;
    }
    return current;
}

// Mark: - Program entry point. Should porbably move to an App class


int main(int argc, char** args) {

    std::setlocale(LC_ALL, "");
    int             fullscreen      = 0;
    uint32_t        width           = 800;
    uint32_t        height          = 600;
    double          timestep        = 60.0;
    long double     startDate       = Physics::julianFromUnix(time(nullptr));
    const char*     jsonpath        = nullptr;
    
    static struct option options[] =
    {
        {"width",       required_argument,  nullptr,        'w'},
        {"height",      required_argument,  nullptr,        'h'},
        {"step",        required_argument,  nullptr,        's'},
        {"start",       required_argument,  nullptr,        'j'},
        {"fullscreen",  no_argument,        &fullscreen,     1 },
        {NULL, 0, NULL, 0}
    };
    
    int c = -1;
    while((c = getopt_long(argc, args, "w:h:s:j:f", options, NULL)) != -1) {
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
            case 'j':
                startDate = std::atof(optarg);
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
    
    time_t seconds = Physics::unixFromJulian(startDate);
    StarSystem system{in, startDate};
    in.close();
    
    Renderer renderer{width, height, jsonpath, static_cast<bool>(fullscreen)};
    
    renderer.onMouseDrag = &onMouseDrag;
    renderer.onMouseScroll = &onMouseScroll;
    renderer.onKeyDown = &onKeyDown;
    
    int64_t bodyID = -1;
    std::vector<std::string> names;
    std::transform(system.bodies().begin(),
                   system.bodies().end(),
                   std::back_inserter(names),
                   [](const StarSystem::Body& b){
        return b.name;
    });
    
    renderer.setScale(100.0 / system.maxDiameter());
    renderer.start([&](Renderer& renderer) {
        
        if(bodySwitcher != 0) {
            bodyID = changeBody(system, renderer, bodyID, bodySwitcher);
            bodySwitcher = 0;
        }
        
        renderer.zoom(scrollSpeed);
        scrollSpeed *= 0.90f;
        if(scrollSpeed > -0.0001f && scrollSpeed < 0.0001f) { scrollSpeed = 0.f; }
        
        if(!showNames) {
            seconds += system.advance(iterations, mult*timestep);
        }
        
        system.render(renderer);
        
        if(showNames) {
            drawBodiyList(renderer, names, bodyID);
        }
        
        renderer.setColor(Renderer::Color::WHITE);
        renderer.drawUIString(Vector3{-0.47, 0.47, 0}, dateString(seconds));
        renderer.drawUIString(Vector3{-0.47, -0.47, 0}, std::to_string(iterations) + " steps/frame");
        return true;
    });
    
    return 0;
}
