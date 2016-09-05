uniform int line_flag;

varying vec3 vpos_m;
varying vec3 vpos_w;
varying vec3 normal_c;
varying vec3 eye_c;
varying vec3 ldir_c;
varying vec3 lpos_w;
varying vec3 tex_color;

void main(void){
	float zz = (vpos_m.z - 2.5) / 4.0;
	vec3 baseColor;
    if (line_flag == 1) {
		vec3 lcol = vec3(1.0,1.0,1.0);
	    baseColor = lcol * zz;
	} else {
	    baseColor = tex_color * zz;
    }
    
    // Light emission properties
	// You probably want to put them as uniforms
	vec3 LightColor = vec3(.75,.75,.85);
	float LightPower = 90000.0f;
	
	// Material properties
	vec3 MaterialDiffuseColor = baseColor;
	vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);

	// Distance to the light
	float distance = length(lpos_w - vpos_w);

	// Normal of the computed fragment, in camera space
	vec3 n = normalize(normal_c);
	// Direction of the light (from the fragment to the light)
	vec3 l = normalize(ldir_c);
	// Cosine of the angle between the normal and the light direction, 
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	//  - light is behind the triangle -> 0
	float cosTheta = clamp(dot(n,l), 0,1);
	
	// Eye vector (towards the camera)
	vec3 E = normalize(eye_c);
	// Direction in which the triangle reflects the light
	vec3 R = reflect(-l,n);
	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = clamp(dot(E,R),0,1);
	
	vec3 color = 
		// Ambient : simulates indirect lighting
		MaterialAmbientColor +
		// Diffuse : "color" of the object
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
		// Specular : reflective highlight, like a mirror
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance);
		
	color=color * exp(-.042*distance); // fog
	gl_FragColor = vec4(color,1.0);
}
