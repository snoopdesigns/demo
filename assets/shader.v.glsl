attribute vec2 coord2d;
varying vec4 graph_coord;
uniform mat4 vertex_transform;

float fract(float x){
	return (x - floor(x));
}
float rand(vec3 co){
    return fract(sin(dot(co,vec3(12.9898,78.233,0))) * 43758.5453);
}

float noise2f(vec3 p){
	vec3 ip = vec3(floor(p[0]),floor(p[1]),0);
	vec3 u = vec3(fract(p[0]),fract(p[1]),0);
	u = u*u*(3.0-2.0*u);
	float res = mix(
		mix(rand(ip),rand(ip+vec3(1.0,0.0,0.0)),u[0]),
		mix(rand(ip+vec3(0.0,1.0,0.0)),rand(ip+vec3(1.0, 1.0,0.0)),u[0]),
		u[1]
	);
	return res*res;
}

float fbm(vec3 c){
	float f = 0.0;
	float Mul = 2;
	float Decay = 0.5;
	float w = 1.0;
	for(int i = 0; i < 19; i++){
		f+= w*noise2f(c);
		c*= Mul;
		w*= Decay; 
	}
	return f; 
}

void main(void) {
	graph_coord = vec4(coord2d, 0, 1);
	graph_coord.z = fbm(vec3(coord2d,0.0));
	gl_Position = vertex_transform * vec4(coord2d, graph_coord.z, 1);
}