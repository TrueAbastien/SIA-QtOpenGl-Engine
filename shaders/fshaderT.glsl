#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform sampler2D texture;
uniform float ka;
uniform float kd;
uniform float ks;
uniform float shininess;
uniform vec3 cam_dir;

varying vec2 v_texcoord;
varying vec3 v_normal;

//! [0]
void main()
{
    vec3 V = normalize(cam_dir);
    vec3 N = normalize(v_normal);

    vec3 R = normalize(reflect(-V, N));
    float k = ka + kd * dot(V, N) + ks * pow(max(dot(R, V), 0.0), shininess);

    // Set fragment color from texture
    gl_FragColor = vec4(k * texture2D(texture, v_texcoord).xyz, 1.0);
}
//! [0]

