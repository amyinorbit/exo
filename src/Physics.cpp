//
//  physics.cpp
//  trappist
//
//  Created by Amy Parent on 06/03/2017.
//  Copyright © 2017 cesarparent. All rights reserved.
//

#include "Physics.hpp"

namespace Physics {
    
    long double gravity(const Vector3& body1, long double mass1,
                        const Vector3& body2, long double mass2) {
        
        auto radius = (body1 - body2).magnitude();
        
        return G * ((mass1*mass2)/(radius*radius));
    }
    
}
