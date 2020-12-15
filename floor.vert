uniform mat4   mvp_matrix;
attribute vec4 a_position;

varying vec2   v_texcoord;

void
main() {
    gl_Position = mvp_matrix * a_position;
    v_texcoord = a_position.xz;
}
