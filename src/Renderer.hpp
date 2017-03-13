//
//  Renderer.hpp
//  trappist
//
//  Created by Amy Parent on 06/03/2017.
//  Copyright © 2017 cesarparent. All rights reserved.
//
#pragma once
#include <cstdint>
#include <string>
#include <functional>
#include <unordered_map>
#include <SDL2/SDL.h>
#include "Math/matrix.hpp"
#include "Math/vec3.hpp"
#include "Model.hpp"

class Renderer {
public:
    
    typedef std::function<bool(Renderer&)> Tick;
    
    enum class Color {
        WHITE,
        BLACK,
        DARKGREY,
        PASTEL_BLUE,
        PURPLE,
        PINK,
        PASTEL_YELLOW,
        KAKI,
        PASTEL_GREEN,
        TURQUOISE,
        LIGHTBLUE,
        YELLOW,
        RED,
        GREEN,
        BLUE,
        INVALID
    };
    
    static const std::unordered_map<std::string, Color> colorNames;
    
    static Color colorNamed(const std::string& name);
    
    std::function<void(Renderer&, SDL_Scancode)>            onKeyDown;
    std::function<void(Renderer&, double dx, double dy)>    onMouseDrag;
    std::function<void(Renderer&, double dx, double dy)>    onMouseScroll;
    
    // MARK: = Renderer Implementation
    
    Renderer(uint32_t width, uint32_t height, const std::string& name);
    
    virtual ~Renderer();
    
    void zoom(double dz);
    
    void rotate(double rx, double ry, double rz);
    
    void setScale(double scale);
    
    Mat44& transform() {
        if(transformDirty_) {
            updateTransform();
            transformDirty_ = false;
        }
        return transform_;
    }
    
    void setColor(const std::string& name, double alpha = 1.0);
    
    void setColor(Color index, double alpha = 1.0);
    
    void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    
    void drawPoint(Vector3 point);
    
    void drawLine(Vector3 start, Vector3 end);
    
    void drawCircle(Vector3 center, double radius);
    
    void drawString(Vector3 start, const std::string& text);
    
    void drawUIString(const Vector3& start, const std::string& text);
    
    void drawModel(const Model& model, const Vector3& start, double scale);
    
    double deltaTime();
    
    void start(Tick updateFn);
    
private:
    
    void updateTransform();
    
    bool t(Vector3& v);
    
    // TRANSFORM STUFF FOR FAKE-Y 3D
    double          rx_, ry_, rz_;
    double          scale_;
    double          zoom_;
    Mat44           transform_;
    Mat44           view_;
    Mat44           proj_;
    bool            transformDirty_;
    
    SDL_Window*     window_;
    SDL_Renderer*   renderer_;
    uint32_t        width_, height_;
    SDL_Color       color_;
    uint32_t        lastFrame_;
    
};
