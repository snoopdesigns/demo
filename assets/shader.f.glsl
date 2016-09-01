varying vec4 graph_coord;
uniform vec4 draw_color;
void main(void){
	gl_FragColor = vec4(0.6, 0.6, 0.6, 1.0) * draw_color;
}
