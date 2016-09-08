uniform int line_flag;
uniform int pp_flag;

varying vec3 vpos_m;
varying vec4 tex_color;

void main(void){
	vec4 baseColor;
    if (line_flag == 1) {
	    baseColor = vec4(0.3,0.3,0.3,1.0);
	} else {
	    baseColor = tex_color;
    }
	gl_FragColor = baseColor;
}
