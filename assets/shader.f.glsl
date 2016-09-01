varying vec4 graph_coord;
uniform vec4 color;
void main(void){
	gl_FragColor = (graph_coord / 2.0 + 0.5) * color;
}