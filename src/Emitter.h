#pragma once
#include "ofMain.h"
#include "Particle.h"

class Emitter {
public:
    vector<Particle> particles;
    glm::vec3 position;
    float rate;        
    glm::vec3 velocity;
    bool oneShot = false;
    float lastSpawnTime = 0;

    void update();
    void draw(ofShader &shader, ofCamera &cam);
    void clear();
    void explode();    
};
