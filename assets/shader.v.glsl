attribute vec2 coord2d;
uniform sampler2D randtexture;
uniform mat4 m;
uniform mat4 v;
uniform mat4 mvp;
uniform vec3 lightpos;
uniform vec3 camerapos;

varying vec3 vpos;

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

float fbm_noise(vec2 p,float o){
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

void main(void) {
	float z = fbm_noise(coord2d, 8);
	gl_Position = mvp * vec4(coord2d, z, 1);
	//vec3 normal = gl_Normal.xyz;
	vpos = vec3(coord2d, z);
}
