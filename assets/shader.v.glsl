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

vec3 no(vec2 p)
{
    //vec2 f=p-floor(p);
    //vec2 u=f*f*f*(f*(f*6-15)+10);
    //float a=(texture2D(randtexture,floor(p) * 0.76/ 2.0 + 0.5).r);
    //float b=(texture2D(randtexture,floor(p) * 0.32/ 2.0 + 0.5).r);
    //float c=(texture2D(randtexture,floor(p) * -0.59/ 2.0 + 0.5).r);
    //float d=(texture2D(randtexture,floor(p) * -0.91/ 2.0 + 0.5).r);
	//return vec3(a+(b-a)*u.x+(c-a)*u.y+(a-b-c+d)*u.x*u.y,30*f*f*(f*(f-2)+1)*(vec2(b-a,c-a)+(a-b-c+d)*u.yx));
	
	float u = p[0]-floor(p[0]);
	float v = p[1]-floor(p[1]);
	float du = 30.0*u*u*(u*(u-2.0)+1.0);
	float dv = 30.0*v*v*(v*(v-2.0)+1.0);
	u = u*u*u*(u*(u*6.0-15.0)+10.0);
	v = u*u*u*(u*(u*6.0-15.0)+10.0);
	float a=(texture2D(randtexture,floor(p) * 0.76/ 2.0 + 0.5).r);
    float b=(texture2D(randtexture,floor(p) * 0.32/ 2.0 + 0.5).r);
    float c=(texture2D(randtexture,floor(p) * -0.59/ 2.0 + 0.5).r);
    float d=(texture2D(randtexture,floor(p) * -0.91/ 2.0 + 0.5).r);
	
	float k0 =  a;
    float k1 =  b - a;
    float k2 =  c - a;
	float k4 =  a - b - c + d;
	
	vec3 res;
	res[0] = k0 + k1*u + k2*v + k4*u*v;
	res[1] = du * (k1 + k4 * v);
	res[2] = du * (k2 + k4 * v);
	return res;
}

float ffff(vec2 p,float o)
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

vec3 calculate_texture_color(vec2 p) {
	return vec3(.55,.65,.75);
}

void main(void) {
	//float z = ffff(coord2d, 10); // Generating terrain height
	//mvp * 
	gl_Position = mvp * vec4(coord2d, ffff(coord2d, 10), 1); // Setting up vertex position
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
