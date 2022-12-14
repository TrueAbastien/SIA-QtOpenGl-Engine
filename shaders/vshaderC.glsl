#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 mvp_matrix;
uniform mat4 model_matrix;

attribute vec4 a_position;
attribute vec3 a_color;
attribute vec3 a_normal;

varying vec3 v_color;
varying vec3 v_normal

//! [0]
void main()
{
    // Calculate vertex position in screen space
    gl_Position = mvp_matrix * a_position;
    
    v_color = a_color;
    v_normal = model_matrix * vec4(a_normal, 0);
}
//! [0]
