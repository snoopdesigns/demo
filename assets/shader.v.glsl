attribute vec2 coord2d;
uniform sampler2D randtexture;
uniform mat4 m;
uniform mat4 v;
uniform mat4 mvp;
uniform vec3 lightpos;
uniform vec3 camerapos;

varying vec3 vpos_m;
varying vec3 tex_color;

float ts = 1.0; //terrain scale, q[2].w
vec3 campos = camerapos; // camera position q[4].xyz
float waterplane = 0.0; // waterplane q[1].w
float season = 128.0/256.0; // season param, q[2].x
vec3 sundir = vec3(-1.0,-1.0,-1.0); // sun direction, q[3]

float random(vec2 p, float m, float n) {
	//p = abs(p);
    p = (floor(p)+vec2(m,n))/256;
	float r01 = texture2D(randtexture, p).r; 
    return r01 * 2.0 - 1.0;
}

// Noise calculation + derivatives
vec3 no(vec2 p)
{
    vec2 f=p-floor(p);
    vec2 u=f*f*f*(f*(f*6-15)+10);
	float a=random(p, 0.0, 0.0);
    float b=random(p, 1.0, 0.0);
    float c=random(p, 0.0, 1.0);
    float d=random(p, 1.0, 1.0);
    return vec3(a+(b-a)*u.x+(c-a)*u.y+(a-b-c+d)*u.x*u.y,30*f*f*(f*(f-2)+1)*(vec2(b-a,c-a)+(a-b-c+d)*u.yx));
}

// Terrain height computation
// Generated value is between 0.0 and 6.0
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

vec3 cn(vec2 p,float e,float o)
{
    float a=f(p,o);
    return normalize(vec3(ts*(a-f(p+vec2(e,0),o)),e,ts*(a-f(p+vec2(0,e),o))));
}

vec3 lerp(vec3 a, vec3 b, float w)
{
  return a + w*(b-a);
}

float lerp(float a, float b, float w)
{
  return a + w*(b-a);
}

float mul(vec3 x, vec3 y) {
    return x.x*y.x + x.y*y.y + x.z*y.z;
}

float saturate(float value) {
	return clamp(value, 0.0, 1.0);
}

vec3 c(vec3 p) {
	// distance
	float t=length(p.xyz-camerapos.xyz);
	vec3 n=cn(p.xy,.001*t,12-log2(t));
	float h=f(3*p.xy,3);
    float r=no(666*p.xy).x;
	vec3 rr = vec3(r,r,r);
    //   rock
	vec3 c=(.1+.75*season)*(.8+.2*rr);
	c=lerp(c,lerp(vec3(.8,.85,.9),vec3(.45,.45,.2)*(.8+.2*r),season),smoothstep(.5-.8*n.y,1-1.1*n.y,h*.15));
	c=lerp(c,lerp(vec3(.37,.23,.08),vec3(.42,.4,.2),season)*(.5+.5*r),smoothstep(0,1,50*(n.y-1)+(h+season)/.4));
	return c;
}

void main(void) {
	//vec3 normal = gl_Normal.xyz;
	
	vpos_m = vec3(0.0,0.0,0.0);
	vpos_m.xy = coord2d;
	vpos_m.z = ts * f(vpos_m.xy, 8);
	//vpos_m.z = 0.0;
	// mvp * 
	gl_Position = mvp * vec4(vpos_m, 1);
	//tex_color = c(vpos_m);
	tex_color = vec3(0.84,0.84,0.84);
}
