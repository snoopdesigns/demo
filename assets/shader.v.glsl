attribute vec2 coord2d;
uniform sampler2D randtexture;
uniform mat4 m;
uniform mat4 v;
uniform mat4 mvp;
uniform vec3 lightpos;
uniform vec3 camerapos;

varying vec3 vpos_m;
varying vec3 vpos_w;
varying vec3 normal_c;
varying vec3 eye_c;
varying vec3 ldir_c;
varying vec3 lpos_w;
varying vec3 tex_color;

float random(vec2 p) {
    return texture2D(randtexture, p).r;
}

// Noise calculation + derivatives
vec3 no(vec2 p)
{
    vec2 f=p-floor(p);
    vec2 u=f*f*f*(f*(f*6-15)+10);
    float a=random((floor(p)+vec2(0,0))/256);
    float b=random((floor(p)+vec2(1,0))/256);
    float c=random((floor(p)+vec2(0,1))/256);
    float d=random((floor(p)+vec2(1,1))/256);
    return vec3(a+(b-a)*u.x+(c-a)*u.y+(a-b-c+d)*u.x*u.y,30*f*f*(f*(f-2)+1)*(vec2(b-a,c-a)+(a-b-c+d)*u.yx));
}

// Terrain height computation
float f(vec2 p,float o)
{
    vec2 d = vec2(0.0,0.0);
    float a=0;
    float b=3;
    for(float i=0;i<o;i++)
    {
        vec3 n=no(.25*p);
        d+=n.yz;
        a+=b*n.x/(1+d.x*d.x+d.y*d.y);
		b=b*0.5;
        p=2.0*p;
    }
    return a;
}

vec3 b(vec3 p) {
    float r=f(666*p.xz, 2);
    vec3 c=vec3(0.85,0.85,0.85)*(.8+.2*r);
    return c;
}

void main(void) {
	//vec3 normal = gl_Normal.xyz;
	vpos_m = vec3(coord2d, f(coord2d, 8));
	gl_Position = mvp * vec4(vpos_m, 1);
	tex_color = b(vpos_m);//vec3(0.84,0.84,0.84);
	
	// Position of the vertex, in worldspace : M * position
	vpos_w = (m * vec4(vpos_m,1)).xyz;
	
	lpos_w = vec3(50,50,50);
	
	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	vec3 vpos_c = (v * m * vec4(vpos_m,1)).xyz;
	eye_c = vec3(0,0,0) - vpos_c;

	// Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
	vec3 lpos_c = (v * vec4(lpos_w,1)).xyz;
	ldir_c = lpos_c + eye_c;
	
	// Normal of the the vertex, in camera space
	normal_c = (v * m * vec4(gl_Normal.xyz,0)).xyz;
}
