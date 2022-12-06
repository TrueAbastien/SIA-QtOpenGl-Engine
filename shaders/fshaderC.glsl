#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

varying vec4 v_color;

//! [0]
void main()
{
    // Set fragment color
    gl_FragColor = vec4(v_color, 1.0);
}
//! [0]

