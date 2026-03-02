#type vertex
#version 330 core

layout(location = 0) in vec3  aPosition;
layout(location = 1) in float aScalar;
layout(location = 2) in float aSelected;

out float vScalar;
out float vSelected;

uniform mat4  u_ProjectionView;
uniform mat4  u_Model;
uniform float u_PointSize;

void main() {
    gl_Position  = u_ProjectionView * u_Model * vec4(aPosition, 1.0);
    gl_PointSize = u_PointSize;
    vScalar      = aScalar;
    vSelected    = aSelected;
}

#type fragment
#version 330 core

in float vScalar;
in float vSelected;
out vec4 fragColor;

uniform int   u_ColorMode;
uniform float uMin;
uniform float uMax;
uniform int   uLabelCount;
uniform float u_Opacity;
uniform vec3  u_SelectionColor;

// Built-in turbo colormap — no texture needed
vec3 turbo(float t) {
    t = clamp(t, 0.0, 1.0);
    vec3 c;
    c.r = 0.1357 + t * (4.5974 + t * (-42.3277 + t * (130.5887 + t * (-150.5666 + t * 58.1375))));
    c.g = 0.0914 + t * (2.1856 + t * (4.8052  + t * (-14.0195 + t * (  4.2522 + t *  2.7747))));
    c.b = 0.1067 + t * (2.6895 + t * (11.0977 + t * (-67.4664 + t * ( 94.9946 + t * -45.0894))));
    return clamp(c, 0.0, 1.0);
}

// Deterministic label color — no palette texture needed
vec3 labelColor(int id) {
    // Golden ratio based HSV spread
    float h = fract(float(id) * 0.61803398875);
    // HSV to RGB: full saturation and value
    vec3 rgb = clamp(abs(mod(h * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);
    return mix(vec3(1.0), rgb, 0.85); // slight desaturation
}

void main() {
    vec4 color = vec4(1.0, 0.0, 1.0, 1.0); // magenta = unhandled mode

    float range = uMax - uMin;
    float t = range > 0.0 ? clamp((vScalar - uMin) / range, 0.0, 1.0) : 0.5;

    if (u_ColorMode == 0) {
        // ScalarField
        color = vec4(turbo(t), 1.0);
    }
    if (u_ColorMode == 2) {
        // ScalarField
        color = vec4(turbo(t), 1.0);
    }
    else if (u_ColorMode == 3) {
        // LabelField
        color = vec4(labelColor(int(floor(vScalar))), 1.0);
    }
    else if (u_ColorMode == 4 || u_ColorMode == 5 || u_ColorMode == 6) {
        // AxisColorX / Y / Z — same path, scalar already holds right axis
        color = vec4(turbo(t), 1.0);
    }

    if (vSelected > 0.5)
        color = vec4(u_SelectionColor, 1.0);

    fragColor = vec4(color.rgb, color.a * u_Opacity);
}