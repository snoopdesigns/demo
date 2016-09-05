uniform int line_flag;

varying vec3 vpos;

vec3 c(vec3 p) {
    return vec3(.55,.65,.75);
}

void main(void){
    if (line_flag == 1) {
	    gl_FragColor = vec4(1.0,1.0,1.0,1.0);    
	} else {
	    gl_FragColor = vec4(c(vpos), 1.0);
    }
}
