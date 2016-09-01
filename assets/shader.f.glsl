varying vec4 graph_coord;
uniform vec4 color;
void main(void){
	vec2 tt = vec2(1.0, 1.0);
	gl_FragColor = vec4(0.6, 0.6, 0.6, 1.0) * color;
}