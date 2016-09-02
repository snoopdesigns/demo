attribute vec2 coord2d;
uniform mat4 m;
uniform mat4 v;
uniform mat4 mvp;
uniform vec3 lightpos;

varying vec3 vertexpos_worldspace;
varying vec3 lightpos_worldspace;
varying vec3 normal_cameraspace;
varying vec3 eyedirection_cameraspace;
varying vec3 lightdirection_cameraspace;

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

float fbm(vec2 p){
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

void main(void) {
	float z = fbm(coord2d); // Generating terrain height
	gl_Position = mvp * vec4(coord2d, z, 1); // Setting up vertex position
	vec3 normal = gl_Normal.xyz;
	
	vertexpos_worldspace = (m * vec4(gl_Position.xyz, 1.0)).xyz; // Position of the vertex, in worldspace : M * position
	vec3 vertexpos_worldspace = ( v * m * vec4(gl_Position.xyz, 1.0)).xyz; // Vector that goes from the vertex to the camera, in camera space.
	eyedirection_cameraspace = vec3(0,0,0) - vertexpos_worldspace;
	vec3 lightpos_cameraspace = ( v * vec4(lightpos,1)).xyz; // Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
	lightdirection_cameraspace = lightpos + eyedirection_cameraspace;
	normal_cameraspace = ( v * m * vec4(normal,0)).xyz; // Normal of the the vertex, in camera space
	lightpos_worldspace = lightpos;
}
