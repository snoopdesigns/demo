attribute vec2 coord2d;
varying vec4 graph_coord;
uniform mat4 vertex_transform;
uniform sampler2D mytexture;

void main(void) {
	graph_coord = vec4(coord2d, 0, 1);
	graph_coord.z = (texture2D(mytexture, graph_coord.xy / 2.0 + 0.5).r);
	mat4 vc = vertex_transform;
	gl_Position = vertex_transform * vec4(coord2d, graph_coord.z, 1);
}
