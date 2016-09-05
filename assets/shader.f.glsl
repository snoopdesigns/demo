uniform int line_flag;

varying vec3 vpos;
varying vec3 tex_color;

void main(void){
    if (line_flag == 1) {
	    gl_FragColor = vec4(0.827,0.827,0.827,1.0);
	} else {
	    gl_FragColor = vec4(tex_color, 1.0);
    }
}
