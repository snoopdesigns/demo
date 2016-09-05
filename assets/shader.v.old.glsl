// MODIFIED FBM
vec3 no(vec2 p)
{
	float u = p[0]-floor(p[0]);
	float v = p[1]-floor(p[1]);
	float du = 30.0*u*u*(u*(u-2.0)+1.0);
	float dv = 30.0*v*v*(v*(v-2.0)+1.0);
	u = u*u*u*(u*(u*6.0-15.0)+10.0);
	v = u*u*u*(u*(u*6.0-15.0)+10.0);
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