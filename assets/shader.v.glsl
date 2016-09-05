attribute vec2 coord2d;
uniform sampler2D randtexture;
uniform mat4 m;
uniform mat4 v;
uniform mat4 mvp;
uniform vec3 lightpos;
uniform vec3 camerapos;

varying vec3 vpos;
varying vec3 tex_color;

float random(vec2 p) {
    return texture2D(randtexture, p).r;
}

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
        p=2.0*p; // TODO multiple to 1.6 etc
    }
    return a;
}

vec3 col(vec3 p) {
    return vec3(0.66,0.66,0.66);
}

void main(void) {
	float z = f(coord2d,8);
	// mvp * 
	gl_Position = mvp * vec4(coord2d, z, 1);
	//vec3 normal = gl_Normal.xyz;
	vpos = vec3(coord2d, z);
	tex_color = col(vpos);
}
