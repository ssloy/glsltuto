#version 120

varying vec3 n;

void main(void) {
    vec3 l = normalize(gl_LightSource[0].position.xyz);

    float intensity = .2 + max(dot(l,normalize(n)), 0.0);

    if (intensity > 0.95)
        intensity = 1;
    else if (intensity > 0.5)
        intensity = .6;
    else if (intensity > 0.25)
        intensity = .4;
    else
        intensity = .2;

    gl_FragColor = gl_Color * intensity;
    return;
}

