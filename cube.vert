uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

attribute vec4 vertexPosition;
attribute vec2 textureCoord;

varying vec2 v_texcoord;

void
main() {
    gl_Position = projection_matrix * view_matrix * model_matrix * vertexPosition;
    v_texcoord = textureCoord;
}
