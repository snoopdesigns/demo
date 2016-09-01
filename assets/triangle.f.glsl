varying vec4 graph_coord;

void main(void) {
	gl_FragColor[0] = graph_coord.z;
    gl_FragColor[1] = 0.0;
    gl_FragColor[2] = 1.0;
}