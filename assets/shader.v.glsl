attribute vec2 coord2d;
uniform sampler2D randtexture;
uniform mat4 m;
uniform mat4 v;
uniform mat4 mvp;
uniform vec3 lightpos;

varying vec3 vertexpos_worldspace;
varying vec3 lightpos_worldspace;
varying vec3 normal_cameraspace;
varying vec3 eyedirection_cameraspace;
varying vec3 lightdirection_cameraspace;

varying vec3 texture_color;

// OLD FBM
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

// MODIFIED FBM
vec3 no(vec2 p)
{
	
	float u = p[0]-floor(p[0]);
	float v = p[1]-floor(p[1]);
	float du = 30.0*u*u*(u*(u-2.0)+1.0);
	float dv = 30.0*v*v*(v*(v-2.0)+1.0);
	u = u*u*u*(u*(u*6.0-15.0)+10.0);
	v = u*u*u*(u*(u*6.0-15.0)+10.0);
	//float a = texture2D(randtexture, p*0.75 / 2.0 + 0.5).r;
	//float b = texture2D(randtexture, p*0.55 / 2.0 + 0.5).r;
	//float c = texture2D(randtexture, p*0.25 / 2.0 + 0.5).r;
	//float d = texture2D(randtexture, p*0.45 / 2.0 + 0.5).r;
	float a = noise(p);
	float b = noise(p);
	float c = noise(p);
	float d = noise(p);
	
	float k0 =  a;
    float k1 =  b - a;
    float k2 =  c - a;
	float k4 =  a - b - c + d;
	
	vec3 res;
	res[0] = k0 + k1*u + k2*v + k4*u*v;
	res[1] = du * (k1 + k4 * v);
	res[2] = dv * (k2 + k4 * u);
	return res;
}

float fbm_mod(vec2 p,float o)
{
    vec2 d=vec2(0.0, 0.0);
	float f=0;
    float w=3;
    for(float i=0;i<o;i++)
    {
        vec3 n=no(.25*p);
        d = d+vec2(n[1], n[2]);
        f = f + w*n[0]/(1+d[0]*d[0]+d[1]*d[1]);
		w = w * 0.5;
		p[0] = 2.0 * p[0];
		p[1] = 2.0 * p[1];
    }
    return f;
}

float fbm_dnoise2f(vec2 p,float o){
	float f=0;
    float w=3;
    for(float i=0;i<o;i++)
    {
        float n=noise(.25*p);
        f = f + w*n;
		w = w * 0.5;
		p[0] = 2.0 * p[0];
		p[1] = 2.0 * p[1];
    }
    return f;
}

vec3 calculate_texture_color(vec2 p) {
	return vec3(.55,.65,.75);
}

void main(void) {
	float z = fbm_mod(coord2d, 20); // Generating terrain height
	//float z = fbm_dnoise2f(coord2d, 10); // fbm with old noise
	//float z = fbm(coord2d); // Generating terrain height
	//float z = no(coord2d)[0];
	//float z = texture2D(randtexture, (floor(coord2d)+vec2(0,0))/256).r;
	//float z = texture2D(randtexture, coord2d / 2.0 + 0.5).r;
	
	gl_Position = mvp * vec4(coord2d, z, 1); // Setting up vertex position
	vec3 normal = gl_Normal.xyz;
	
	vertexpos_worldspace = (m * vec4(gl_Position.xyz, 1.0)).xyz; // Position of the vertex, in worldspace : M * position
	vec3 vertexpos_worldspace = ( v * m * vec4(gl_Position.xyz, 1.0)).xyz; // Vector that goes from the vertex to the camera, in camera space.
	eyedirection_cameraspace = vec3(0,0,0) - vertexpos_worldspace;
	vec3 lightpos_cameraspace = ( v * vec4(lightpos,1)).xyz; // Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
	lightdirection_cameraspace = lightpos + eyedirection_cameraspace;
	normal_cameraspace = ( v * m * vec4(normal,0)).xyz; // Normal of the the vertex, in camera space
	lightpos_worldspace = lightpos;
	texture_color = calculate_texture_color(coord2d);
}
