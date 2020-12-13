uniform mat4 mvp_matrix;
attribute vec3 a_position;
varying vec3 vTexCoord;

void
main(void) {
    gl_Position = mvp_matrix * vec4(a_position, 1.0);
    vTexCoord   = a_position;
}
