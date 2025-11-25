#version 120

void main() {
    
    float dist = length(gl_PointCoord - vec2(0.5));
    float alpha = 1.0 - smoothstep(0.0, 0.5, dist);   // soft circle

    gl_FragColor = vec4(1.0, 0.5, 0.1, alpha);  // orange rocket flame
}
