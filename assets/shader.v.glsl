attribute vec2 coord2d;
varying vec4 graph_coord;
uniform mat4 vertex_transform;

float rand(vec2 n) { 
    return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

float noise(vec2 p){
    vec2 ip = floor(p);
    vec2 u = fract(p);
    u = u*u*(3.0-2.0*u);

    float res = mix(
        mix(rand(ip),rand(ip+vec2(1.0,0.0)),u.x),
        mix(rand(ip+vec2(0.0,1.0)),rand(ip+vec2(1.0,1.0)),u.x),u.y);
    return res*res;
}

const mat2 m2 = mat2(0.8,-0.6,0.6,0.8);
float fbm( in vec2 p ){
    float f = 0.0;
    f = 0.5000000f*noise(vec2(1.0f*p[0], 1.0f*p[1]));
	f += 0.2500000f*noise(vec2(2.0f*p[0], 2.0f*p[1]));
	f += 0.1250000f*noise(vec2(4.0f*p[0], 4.0f*p[1]));
	f += 0.0625000f*noise(vec2(8.0f*p[0], 8.0f*p[1]));
	f += 0.0312500f*noise(vec2(16.0f*p[0], 16.0f*p[1]));
	f += 0.0156250f*noise(vec2(32.0f*p[0], 32.0f*p[1]));
	f += 0.0078125f*noise(vec2(64.0f*p[0], 64.0f*p[1]));
    return f/0.9375;
}

float fbm2(vec2 p) {
	int ioct = 5;
	float x = p[0];
	float y = p[1];
	float f = 0.0;
    float w = 0.5;
    float dx = 0.0;
    float dy = 0.0;
    for( int i=0; i < ioct ; i++ )
    {
        f += w * noise(vec2(x,y));
		w *= 0.5;
		x *= 2.0;
		y *= 2.0;
    }
    return f;
}

void main(void) {
	graph_coord = vec4(coord2d, 0, 1);
	graph_coord.z = fbm(coord2d);
	gl_Position = vertex_transform * vec4(coord2d, graph_coord.z, 1);
}
