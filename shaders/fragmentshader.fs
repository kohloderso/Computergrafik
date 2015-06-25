#version 330

const int NUMLIGHTS = 2;

uniform float ambientIntensity = 0.5;
uniform float shininess = 1;
uniform vec3 LightColorFixed;
uniform vec3 LightColorMoving;
uniform vec3 LightPositionFixed;
uniform vec3 LightPositionMoving;
uniform float LightPower[NUMLIGHTS];
uniform bool enableTexture = true;

uniform sampler2D myTextureSampler;

uniform bool ambientOn = true;
uniform bool diffuseOn = true;
uniform bool specularOn = true;
uniform bool billBoard = true;

// Interpolated values from the vertex shaders
in vec3 vColor;
in vec4 Position_worldspace;
in vec4 Normal_worldspace;
in vec4 EyeDirection_worldspace;
in vec2 UV;

out vec4 FragColor;

void main()
{
	vec3 LightColor;
	vec3 LightPosition;
	vec4 LightDirection_worldspace[NUMLIGHTS];
	vec3 groundColor;
	if(enableTexture) {
	    groundColor = vec3(texture( myTextureSampler, UV ));
	} else {
	    groundColor = vColor;
	}
	vec4 color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	if(ambientOn) {
		color += ambientIntensity * vec4(groundColor, 1.0f); //vec4(vColor, 1.0f);
	}
	for(int i = 0; i < NUMLIGHTS; i++) {
		if(i == 0) {
			LightColor = LightColorMoving;
			LightPosition = LightPositionMoving;
		} else {
			LightColor = LightColorFixed;
			LightPosition = LightPositionFixed;
		}
		
		LightDirection_worldspace[i] = vec4(LightPosition, 1.0f) - Position_worldspace;
	
		// Distance to the light
		float distance = length( vec4(LightPosition, 1) - Position_worldspace );

		// Normal of the computed fragment, in camera space
		vec4 normal = normalize( Normal_worldspace );
		// Direction of the light (from the fragment to the light)
		vec4 light = normalize( LightDirection_worldspace[i] );
		// Cosine of the angle between the normal and the light direction, 
		// clamped above 0
		//  - light is at the vertical of the triangle -> 1
		//  - light is perpendicular to the triangle -> 0
		//  - light is behind the triangle -> 0
		float cosTheta = clamp( dot( normal,light ), 0,1 );
	
		// Eye vector (towards the camera)
		vec4 eyevector = normalize(EyeDirection_worldspace);
		// Direction in which the triangle reflects the light
		vec4 reflection = reflect(-light,normal);
		// Cosine of the angle between the Eye vector and the Reflect vector,
		// clamped to 0
		//  - Looking into the reflection -> 1
		//  - Looking elsewhere -> < 1
		float cosAlpha = clamp( dot( eyevector,reflection ), 0,1 );

		if(diffuseOn) {
			color += vec4(groundColor, 1.0f) * vec4(LightColor, 1.0f) * LightPower[i] * cosTheta/(distance*distance);
		}
		if(specularOn) {
			color += vec4(LightColor, 1.0f) * LightPower[i] * pow(cosAlpha,shininess) / (distance*distance);
		}
	}
	FragColor = color;
	if(billBoard) {
		FragColor = vec4(groundColor, 0.3);
	}

}
