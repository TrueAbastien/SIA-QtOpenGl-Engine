#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform float ka;
uniform float kd;
uniform float ks;
uniform float shininess;
uniform vec3 cam_dir;

varying vec3 v_color;
varying vec3 v_normal;

//! [0]
void main()
{
    vec3 V = normalize(cam_dir);
    vec3 N = normalize(v_normal);

    vec3 R = normalize(reflect(-V, N));
    vec3 col = v_color * (ka + kd * dot(V, N) + ks * pow(max(dot(R, V), 0.0), shininess));

    // Set fragment color
    gl_FragColor = vec4(col, 1.0);
}
//! [0]

