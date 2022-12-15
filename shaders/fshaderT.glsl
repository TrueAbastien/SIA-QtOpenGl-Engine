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

varying vec2 v_texcoord;
varying vec3 v_normal;
varying vec3 v_camDir;

//! [0]
void main()
{
    vec3 L = normalize(v_camDir);
    vec3 N = normalize(v_normal);

    vec3 R = normalize(reflect(-L, N));
    float k = ka + kd * dot(L, N) + ks * pow(max(dot(R, v_camDir), 0.0), shininess);

    // Set fragment color from texture
    gl_FragColor = vec4(k * texture2D(texture, v_texcoord).xyz, 1.0);
}
//! [0]

