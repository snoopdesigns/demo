uniform int line_flag;

varying vec3 vpos_m;
varying vec3 vpos_w;
varying vec3 normal_c;
varying vec3 eye_c;
varying vec3 ldir_c;
varying vec3 lpos_w;
varying vec3 tex_color;

void main(void){
	vec3 baseColor;
    if (line_flag == 1) {
	    baseColor = vec3(0.3,0.3,0.3);
	} else {
	    baseColor = tex_color;
    }
	gl_FragColor = vec4(baseColor,1.0);
}
