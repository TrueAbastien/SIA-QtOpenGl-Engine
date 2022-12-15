#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform float ka;
uniform float kd;
uniform float ks;
uniform float shininess;

varying vec3 v_color;
varying vec3 v_normal;
varying vec3 v_camDir;

//! [0]
void main()
{
    vec3 L = normalize(v_camDir);
    vec3 N = normalize(v_normal);

    vec3 R = normalize(reflect(-L, N));
    vec3 col = v_color * (ka + kd * dot(L, N) + ks * pow(max(dot(R, v_camDir), 0.0), shininess));

    // Set fragment color
    gl_FragColor = vec4(col, 1.0);
}
//! [0]

