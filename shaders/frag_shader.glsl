#version 120

uniform vec4 viewport;

varying mat4 VPMTInverse;
varying mat4 VPInverse;
varying vec3 centernormclip;

void main(void) {
    vec4 c3 = VPMTInverse[2];
    vec4 xpPrime = VPMTInverse*vec4(gl_FragCoord.x, gl_FragCoord.y, 0.0, 1.0);

    float c3TDc3 = dot(c3.xyz, c3.xyz)-c3.w*c3.w;
    float xpPrimeTDc3 = dot(xpPrime.xyz, c3.xyz)-xpPrime.w*c3.w;
    float xpPrimeTDxpPrime = dot(xpPrime.xyz, xpPrime.xyz)-xpPrime.w*xpPrime.w;

    float square = xpPrimeTDc3*xpPrimeTDc3 - c3TDc3*xpPrimeTDxpPrime;
    if (square<0.0) {
        discard;
    } else {
        float z = ((-xpPrimeTDc3-sqrt(square))/c3TDc3);
        gl_FragDepth = z;

        vec4 pointclip = VPInverse*vec4(gl_FragCoord.x, gl_FragCoord.y, z, 1);
        vec3 pointnormclip = vec3(pointclip)/pointclip.w;

        vec3 lightDir = normalize(vec3(gl_LightSource[0].position));
        float intensity = .2 + max(dot(lightDir,normalize(pointnormclip-centernormclip)), 0.0);
        gl_FragColor = intensity*gl_Color;
    }
}

