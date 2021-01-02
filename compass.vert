uniform mat4   mvp_matrix;
attribute vec4 vertexPosition;

varying vec2   v_texcoord;

void
main() {
    gl_Position = mvp_matrix * vertexPosition;
    v_texcoord = vertexPosition.xy;
}
