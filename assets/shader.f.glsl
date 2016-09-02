varying vec3 vertexpos_worldspace;
varying vec3 lightpos_worldspace;
varying vec3 normal_cameraspace;
varying vec3 eyedirection_cameraspace;
varying vec3 lightdirection_cameraspace;
varying vec3 texture_color;
uniform vec4 draw_color;

void main(void){

	vec3 LightColor = vec3(1,1,1); // Light emission properties
	float LightPower = 70.0f;
	
	// Material properties
	vec3 MaterialDiffuseColor = texture_color;//vec3(0.6, 0.6, 0.6);
	vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);
	
	float distance = length( lightpos_worldspace - vertexpos_worldspace ); // Distance to the light
	
	// Calculating angles between normals to the fragment and light direction
	vec3 n = normalize( normal_cameraspace );
	vec3 l = normalize( lightdirection_cameraspace );
	float cosTheta = clamp( dot( n,l ), 0,1 );
	vec3 E = normalize(eyedirection_cameraspace);
	vec3 R = reflect(-l,n);
	float cosAlpha = clamp( dot( E,R ), 0,1 );
	
	vec3 color = 
		MaterialAmbientColor +
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance);
	gl_FragColor = vec4(color, 1.0) * draw_color;
}
