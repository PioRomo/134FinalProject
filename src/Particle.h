#pragma once
#include "ofMain.h"

class Particle {
public:
    glm::vec3 pos, vel;
    float lifespan;     
    float birthtime; 

    Particle(glm::vec3 p, glm::vec3 v) {
        pos = p;
        vel = v;
        lifespan = 1.0;     
        birthtime = ofGetElapsedTimef();
    }

    float age() { return ofGetElapsedTimef() - birthtime; }
};
