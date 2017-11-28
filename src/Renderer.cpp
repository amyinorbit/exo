//
//  Renderer.cpp
//  trappist
//
//  Created by Amy Parent on 06/03/2017.
//  Copyright © 2017 Amy Parent. All rights reserved.
//
#include <cmath>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "Renderer.hpp"
#include "Math/Transform.hpp"

static const SDL_Color colors[] {
    SDL_Color{255, 255, 255, 255},  // WHITE
    SDL_Color{0,   0,   0,   255},  // BLACK
    SDL_Color{20,  20,  20,  255},  // DARKGREY
    SDL_Color{100,  70,  200, 255},  // BLUE
    SDL_Color{192, 40,  152, 255},  // PURPLE
    SDL_Color{230, 84,  112, 255},  // PINK
    SDL_Color{226, 201, 110, 255},  // YELLOW
    SDL_Color{156, 154, 40,  255},  // KAKI
    SDL_Color{0,   124, 53,  255},  // GREEN
    SDL_Color{0,   176, 156, 255},  // TURQUOISE
    SDL_Color{83,  208, 241, 255},  // LIGHTBLUE
    SDL_Color{255, 255, 0,   255},  // YELLOW
    SDL_Color{255, 0,   0,   255},  // RED
    SDL_Color{0,   255, 0,   255},  // GREEN
    SDL_Color{0,   0,   255, 255},  // BLUE
    
};

const std::unordered_map<std::string, Renderer::Color> Renderer::colorNames = {
    {"WHITE", Color::WHITE},
    {"BLACK", Color::BLACK},
    {"DARKGREY", Color::DARKGREY},
    {"PASTEL_BLUE", Color::PASTEL_BLUE},
    {"PURPLE", Color::PURPLE},
    {"PINK", Color::PINK},
    {"PASTEL_YELLOW", Color::PASTEL_YELLOW},
    {"KAKI", Color::KAKI},
    {"PASTEL_GREEN", Color::PASTEL_GREEN},
    {"TURQUOISE", Color::TURQUOISE},
    {"LIGHTBLUE", Color::LIGHTBLUE},
    {"YELLOW", Color::YELLOW},
    {"RED", Color::RED},
    {"GREEN", Color::GREEN},
    {"BLUE", Color::BLUE},
};

Renderer::Renderer(uint32_t width, uint32_t height, const std::string& name, bool fullscreen)
: center_(0, 0, 0)
, nextCenter_(NULL)
, prevCenter_(NULL)
, centerT_(1.f)
, rx_(0)
, ry_(0)
, rz_(0)
, scale_(1.0)
, zoom_(1.0)
, transform_(Mat44::identity())
, view_(Mat44::identity())
, proj_(Mat44::identity())
, transformDirty_(true)
, window_(nullptr)
, renderer_(nullptr)
, width_(width)
, height_(height)
, color_(SDL_Color{0, 0, 0, 255})
, lastFrame_(0) {
    if(SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO) < 0) {
        throw "error initialising graphics library";
    }
    
    uint32_t flags = SDL_WINDOW_ALLOW_HIGHDPI;
    if(fullscreen) {
        SDL_DisplayMode current;
        flags |= SDL_WINDOW_FULLSCREEN;
        if(SDL_GetCurrentDisplayMode(0, &current) < 0) {
            throw "error fetching screen size";
        }
        width_ = current.w;
        height_ = current.h;
    } else {
        flags |= SDL_WINDOW_SHOWN;
    }
    
    window_ = SDL_CreateWindow(("exo - simulating '" + name + "'").c_str(),
                               SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                               width_, height_, flags);
    if(!window_) {
        throw "error creating display window";
    }
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!renderer_) {
        throw "error creating graphics renderer";
    }
    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
    
    proj_ = Transform::projection(60, double(width_)/double(height_), 1.0, 800);
    view_ = Transform::translate(width_/2, height_/2, 0)
            * Transform::scale(width_, height_, 1.0);
}

Renderer::~Renderer() {
    if(renderer_) {
        SDL_DestroyRenderer(renderer_);
    }
    if(window_) {
        SDL_DestroyWindow(window_);
    }
    SDL_Quit();
}

void Renderer::setScale(double scale) {
    scale_ = scale;
    transformDirty_ = true;
}

void Renderer::zoom(double dz) {
    zoom_ += dz;
    if(zoom_ < 0) { zoom_ = 0; }
    transformDirty_ = true;
}

void Renderer::rotate(double rx, double ry, double rz) {
    rx_ += radians(rx);
    ry_ += radians(ry);
    rz_ += radians(rz);
    transformDirty_ = true;
}

void Renderer::updateTransform() {
    
    transform_ = // Our view matrix is basic. The camera is at 0, 0, zoom_*height
                //Transform::translate(0, 0, -100) *
                 Transform::translate(0, 0, -(1.0 + 10.0*(zoom_*zoom_*zoom_))) *
                 // This is basically the model matrix
                 Transform::rotateX(rx_) *
                 Transform::rotateY(ry_) *
                 Transform::rotateZ(rz_) *
                 Transform::scale(scale_) *
                 Transform::translate(-center_.x, -center_.y, -center_.z);
                 
    transformDirty_ = false;
}

bool Renderer::t(Vector3& v) {
    v = Transform::apply(transform(), v);
    matrix<4,1> xyzw {
        v.x,
        v.y,
        v.z,
        1.0
    };
    auto r = proj_ * xyzw;
    
    v.x = r[0][0];
    v.y = -r[1][0];
    v.z = r[2][0];
    auto w = r[3][0];
    
    if((v.x < -w || v.x > w)
       || (v.y < -w || v.y > w)
       || (v.z < -w || v.z > w)) { return false; }
    
    v /= w;
    v = Transform::apply(view_, v);
    return true;
}

Renderer::Color Renderer::colorNamed(const std::string& name) {
    if(colorNames.count(name) == 0) { return Color::LIGHTBLUE; }
    return colorNames.at(name);
}

void Renderer::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(renderer_, r, g, b, a);
    color_ = SDL_Color{r, g, b, a};
}

void Renderer::setColor(const std::string& name, double alpha) {
    if(colorNames.count(name) == 0) { return; }
    setColor(colorNames.at(name));
}

void Renderer::setColor(Color index, double alpha) {
    int idx = (int)index;
    SDL_SetRenderDrawColor(renderer_, colors[idx].r, colors[idx].g, colors[idx].b, alpha * colors[idx].a);
    color_ = SDL_Color{colors[idx].r, colors[idx].g, colors[idx].b, Uint8(alpha * colors[idx].a)};
}

void Renderer::drawPoint(Vector3 point) {
    if(!t(point)) { return; }
    SDL_RenderDrawPoint(renderer_, point.x, point.y);
}

void Renderer::drawLine(Vector3 start, Vector3 end) {
    if(!t(start) || !t(end)) { return; }
    aalineRGBA(renderer_, start.x, start.y, end.x, end.y, color_.r, color_.g, color_.b, color_.a);
    
}

void Renderer::drawCircle(Vector3 center, double radius) {
    if(!t(center)) { return; }
    aacircleRGBA(renderer_, center.x, center.y, radius, color_.r, color_.g, color_.b, color_.a);
}

void Renderer::drawString(Vector3 start, const std::string& text) {
    if(!t(start)) { return; }
    stringRGBA(renderer_, start.x, start.y, text.c_str(), color_.r, color_.g, color_.b, color_.a);
}

void Renderer::drawUIString(const Vector3& start, const std::string& text) {
    auto s = Transform::apply(view_, start);
    stringRGBA(renderer_, s.x, s.y, text.c_str(), color_.r, color_.g, color_.b, color_.a);
}


void Renderer::drawUIBox(Vector3 topLeft, Vector3 size, Color background, Color border) {
    auto oldColor = color_;
    
    const auto scaler = Vector3{float(width_), float(height_), 0.0};
    topLeft = Transform::apply(view_, topLeft);
    size *= scaler;
    
    SDL_Rect box = {int(topLeft.x), int(topLeft.y), int(size.x), int(size.y)};
    
    setColor(background);
    SDL_RenderFillRect(renderer_, &box);
    setColor(border);
    SDL_RenderDrawRect(renderer_, &box);
    
    color_ = oldColor;
}


void Renderer::drawModel(const Model& model, const Vector3& start, double scale) {
    std::vector<Vector3> vertices;
    for(auto& v : model.vertices) {
        vertices.push_back(start + scale*v);
    }
    for(auto& face : model.faces) {
        //drawPoint(vertices[face.a]);
        //drawPoint(vertices[face.b]);
        //drawPoint(vertices[face.c]);
        //drawPoint(vertices[face.d]);
        //drawLine(vertices[face.a], vertices[face.b]);
        drawLine(vertices[face.b], vertices[face.c]);
        drawLine(vertices[face.c], vertices[face.d]);
        drawLine(vertices[face.d], vertices[face.a]);
    }
}

double Renderer::deltaTime() {
    return static_cast<double>(SDL_GetTicks() - lastFrame_) / 1000.0;
}

#define EASE_INOUT(t) (-0.5f*(std::cos((t) * M_PI)-1))

void Renderer::updateCenterTransition(double deltaT) {
    if(nextCenter_ == NULL) {
        if(prevCenter_) {
            center_ = *prevCenter_;
            transformDirty_ = true;
        }
        return;
    }
    if(centerT_ >= 1.f) { return; }
    
    Vector3 A = prevCenter_ ? *prevCenter_ : Vector3{0, 0, 0};
    
    centerT_ += deltaT;
    center_ = lerp(A, *nextCenter_, EASE_INOUT(centerT_));
    transformDirty_ = true;
    
    if(centerT_ >= 1.f) {
        centerT_ = 1.f;
        prevCenter_ = nextCenter_;
        nextCenter_ = NULL;
    }
}

void Renderer::start(Tick updateFn) {
    SDL_Event e;
    bool quit = false;
    
    while(!quit) {
        while(SDL_PollEvent( &e ) != 0) {
            switch(e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    if(onKeyDown) {
                        onKeyDown(*this, e.key.keysym.scancode);
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if((e.motion.state & SDL_BUTTON_LMASK) && onMouseDrag) {
                        onMouseDrag(*this, double(e.motion.xrel) / double(width_),
                                           double(e.motion.yrel) / double(height_));
                    }
                    break;
                case SDL_MOUSEWHEEL:
                    if(onMouseScroll) {
                        onMouseScroll(*this, double(e.wheel.x) / double(width_),
                                             double(e.wheel.y) / double(height_));
                    }
                    break;
            }
        }
        
        int interval = SDL_GetTicks() - lastFrame_;
        if(interval < 16) {
            SDL_Delay(16 - interval);
            interval = 16;
        }
        
        
        setColor(Color::DARKGREY);
        SDL_RenderClear(renderer_);
        
        if(!updateFn(*this)) { quit = true; }
        
        setColor(Color::RED);
        drawLine(Vector3{0, 0, 0}, Vector3{.1/scale_, 0, 0});
        
        setColor(Color::GREEN);
        drawLine(Vector3{0, 0, 0}, Vector3{0, .1/scale_, 0});
        
        
        setColor(Color::BLUE);
        drawLine(Vector3{0, 0, 0}, Vector3{0, 0, .1/scale_});
        
        SDL_RenderPresent(renderer_);
        updateCenterTransition(float(interval) / 1000.f);
        lastFrame_ = SDL_GetTicks();
    }
}

