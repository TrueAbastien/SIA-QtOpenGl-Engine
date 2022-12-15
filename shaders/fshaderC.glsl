#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

varying vec3 v_color;
varying vec3 v_normal;
varying vec3 v_camDir;

//! [0]
void main()
{
    vec3 L = normalize(vec3(1.0, 1.0, 1.0));
    vec3 N = normalize(v_normal);

    vec3 R = normalize(reflect(-L, N));
    vec3 col = v_color * (1.0 + 1.0 * max(dot(L, N), 0.0) + 1.0 * pow(max(dot(R, v_camDir), 0.0), 32.0));

    // Set fragment color
    //gl_FragColor = vec4(col, 1.0);
    gl_FragColor = vec4(col, 1.0);
}
//! [0]

