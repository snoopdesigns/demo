varying vec4 graph_coord;
varying vec3 Position_worldspace;
varying vec3 LightPosition_worldspace;
varying vec3 Normal_cameraspace;
varying vec3 EyeDirection_cameraspace;
varying vec3 LightDirection_cameraspace;
uniform vec4 draw_color;

void main(void){

	vec3 LightColor = vec3(1,1,1);
	float LightPower = 50.0f;
	vec3 MaterialDiffuseColor = vec3(0.6, 0.6, 0.6);
	vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);
	float distance = length( LightPosition_worldspace - Position_worldspace );
	vec3 n = normalize( Normal_cameraspace );
	vec3 l = normalize( LightDirection_cameraspace );
	float cosTheta = clamp( dot( n,l ), 0,1 );
	vec3 E = normalize(EyeDirection_cameraspace);
	vec3 R = reflect(-l,n);
	float cosAlpha = clamp( dot( E,R ), 0,1 );
	
	vec3 color = 
		MaterialAmbientColor +
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance);
	gl_FragColor = vec4(color, 1.0) * draw_color;
}
