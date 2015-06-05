#version 330

const int NUMLIGHTS = 2;

uniform vec3 LightPositions[NUMLIGHTS];
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat4 InverseTransposeModel;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;
layout (location = 2) in vec3 vertexNormal;

out vec3 vColor;
out vec4 Position_worldspace;
out vec4 Normal_worldspace;
out vec4 EyeDirection_worldspace;
out vec4 LightDirection_worldspace[NUMLIGHTS];
out vec4 LightColor[NUMLIGHTS];
out vec4 LightPower[NUMLIGHTS];


void main()
{
   	gl_Position = ProjectionMatrix*ViewMatrix*ModelMatrix*vec4(Position.x, Position.y, Position.z, 1.0);

	// Position of the vertex, in worldspace 
	Position_worldspace = ModelMatrix * vec4(Position,1);
	
	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	//vec4 vertexPosition_cameraspace = ViewMatrix * ModelMatrix * vec4(Position,1);
	//EyeDirection_cameraspace = vec4(0,0,0,1.0) - vertexPosition_cameraspace;
	EyeDirection_worldspace = vec4(0,0,0, 1.0) - Position_worldspace;

	for(int i = 0; i < NUMLIGHTS; i++) {
		// Vector that goes from the vertex to the light
		LightDirection_worldspace[i] = vec4(LightPositions[i], 1.0f) - Position_worldspace;
	}
	
	// Normal of the the vertex, in world space
	//Normal_cameraspace = inverse(transpose(ViewMatrix * ModelMatrix)) * vec4(vertexNormal,0); // Use  inverse transpose because of non-uniform-scaling
	//Normal_cameraspace = ( inverse(transpose(ModelMatrix)) * vec4(vertexNormal,0)).xyz;
	Normal_worldspace = InverseTransposeModel * vec4(vertexNormal, 0.0f);
   	vColor = Color;
}
