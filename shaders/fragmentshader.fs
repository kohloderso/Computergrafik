#version 330

uniform vec4 colorUniform = vec4(0.5, 0.5, 0.5, 1.0);
uniform int getColor = 0;

// Interpolated values from the vertex shaders
in vec3 vColor;
in vec4 Position_worldspace;
in vec4 Normal_cameraspace;
in vec4 EyeDirection_cameraspace;
in vec4 LightDirection_cameraspace;

out vec4 FragColor;

uniform vec3 LightPosition_worldspace;

void main()
{
    // Light emission properties
	// You probably want to put them as uniforms
	vec3 LightColor = vec3(1,1,1);
	float LightPower = 10.0f;
	
	// Material properties
	vec3 MaterialDiffuseColor = vColor;
	vec3 MaterialAmbientColor = vec3(0.5,0.5,0.5) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);

	// Distance to the light
	float distance = length( vec4(LightPosition_worldspace, 1) - Position_worldspace );

	// Normal of the computed fragment, in camera space
	vec4 n = normalize( Normal_cameraspace );
	// Direction of the light (from the fragment to the light)
	vec4 l = normalize( LightDirection_cameraspace );
	// Cosine of the angle between the normal and the light direction, 
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	//  - light is behind the triangle -> 0
	float cosTheta = clamp( dot( n,l ), 0,1 );
	
	// Eye vector (towards the camera)
	vec4 E = normalize(EyeDirection_cameraspace);
	// Direction in which the triangle reflects the light
	vec4 R = reflect(-l,n);
	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = clamp( dot( E,R ), 0,1 );
	
	FragColor = vec4(
		// Ambient : simulates indirect lighting
		MaterialAmbientColor +
		// Diffuse : "color" of the object
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
		// Specular : reflective highlight, like a mirror
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,2) / (distance*distance)
		,0);
	//FragColor = vec4(vColor, 1);

}
