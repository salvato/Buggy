uniform samplerCube uTexture;
varying vec3 vTexCoord;

void
main(void) {
    gl_FragColor = vec4(0.2, 0.4, 0.6, 0.3);//textureCube(uTexture, vTexCoord);
}
