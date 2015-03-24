#version 120

uniform vec4 viewport;
varying float radius;
varying vec2  center;


void main(void) {
    vec2 ndc_current_pixel = ((2.0 * gl_FragCoord.xy) - (2.0 * viewport.xy)) / (viewport.zw) - 1;

    vec2 diff = ndc_current_pixel - center;
    float d2 = dot(diff,diff);
    float r2 = radius*radius;

    if (d2>r2) {
        discard;
    } else {
        vec3 l = normalize(gl_LightSource[0].position.xyz);
        float dr =  sqrt(r2-d2);
        vec3 n = vec3(ndc_current_pixel-center, dr);
        float intensity = .2 + max(dot(l,normalize(n)), 0.0);
        gl_FragColor = gl_Color*intensity;
        gl_FragDepth = gl_FragCoord.z + dr*gl_DepthRange.diff/2.0*gl_ProjectionMatrix[2].z;
    }
}

