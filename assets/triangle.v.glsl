attribute vec2 coord2d;
varying vec4 graph_coord;
uniform mat4 mvp;

void main(void) {
	graph_coord = vec4(coord2d, 0, 1);
	graph_coord.z = graph_coord.x;

	gl_Position = mvp * vec4(coord2d, graph_coord.z, 1);
}