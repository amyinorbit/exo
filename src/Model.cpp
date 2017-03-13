#include "Model.hpp"

Model Model::Sphere(int LATS, int LONS) {
    Model sphere;
    
    double phi_inc = M_PI / double(LATS+1);
    double theta_inc = (2.0 * M_PI) / double(LONS);
    double phi = -M_PI/2.0;
    
    // Vertices first
    for(int i = 0; i < LATS; ++i) {
        phi += phi_inc;
        double r = std::cos(phi);
        double z = std::sin(phi);
        for(int j = 0; j < LONS; ++j) {
            sphere.vertices.push_back(0.5 * Vector3{
                r * std::cos(theta_inc * j),
                r * std::sin(theta_inc * j),
                z
            });
        }
    }
    // Add top and bottom vertices
    sphere.vertices.push_back(Vector3{0, 0, 0.5});
    sphere.vertices.push_back(Vector3{0, 0, -0.5});
    
    int top = sphere.vertices.size()-2, bottom = sphere.vertices.size()-1;
    
    // create faces. JOY.
    // The top and bottom rows first, 
    
    int prev = LONS-1;
    int offset = (LATS-1)*LONS;
    for(int i = 0; i < LONS; ++i) {
        sphere.faces.push_back(Face{
            bottom, bottom, prev, i
        });
        
        sphere.faces.push_back(Face{
            top, top, offset+prev, offset+i
        });
        prev = i;
    }
    
    // now the rest, should be easier.
    for(int i = 1; i < LATS; ++i) {
        int currOffset = i * LONS;
        int prevOffset = (i-1) * LONS;
        
        prev = LONS-1;
        for(int j = 0; j < LONS; ++j) {
            sphere.faces.push_back(Face{
                currOffset + prev, currOffset + j,
                prevOffset + j, prevOffset + prev
            });
            prev = j;
        }
    }
    
    return sphere;
}

const Model& Model::sphereInstance() {
    static Model instance_ = Sphere();
    return instance_;
}

