uniform mat4   mvp_matrix;
uniform vec4   color;
attribute vec4 vertexPosition;

varying vec4   v_color;

void
main() {
    gl_Position = mvp_matrix * vertexPosition;
    v_color = color;
}
