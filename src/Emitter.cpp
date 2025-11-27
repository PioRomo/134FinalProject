#include "Emitter.h"
#include "ofGraphics.h"

void Emitter::update() {
    float t = ofGetElapsedTimef();

    //Continous exhause
    if (!oneShot) {
        if (t - lastSpawnTime > 1.0f / rate) {
            particles.emplace_back(
                position,
                velocity + glm::vec3(ofRandom(-1,1), ofRandom(-1,1), ofRandom(-1,1))
            );
            lastSpawnTime = t;
        }
    }

    //Remove dead particles
    particles.erase(
        remove_if(
            particles.begin(),
            particles.end(),
            [&](Particle &p) { return p.age() > p.lifespan; }
        ),
        particles.end()
    );

    //Update particle positions
    float dt = ofGetLastFrameTime();
    for (auto &p : particles) {
        p.pos += p.vel * dt;
        p.vel *= 0.99;
    }
}

void Emitter::draw(ofShader &shader, ofCamera &cam) {
    shader.begin();

    shader.setUniformMatrix4f("modelViewProjectionMatrix", cam.getModelViewProjectionMatrix());


    glBegin(GL_POINTS);
    for (auto &p : particles) {
        shader.setUniform1f("age", p.age());
        glVertex3f(p.pos.x, p.pos.y, p.pos.z);
    }
    glEnd();

    shader.end();
}
