#version 330

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
//uniform mat4 InverseTransposeMV;
uniform vec3 LightPosition_worldspace;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;
layout (location = 2) in vec3 vertexNormal;

out vec3 vColor;
out vec4 Position_worldspace;
out vec4 Normal_cameraspace;
out vec4 EyeDirection_cameraspace;
out vec4 LightDirection_cameraspace;


void main()
{
   	gl_Position = ProjectionMatrix*ViewMatrix*ModelMatrix*vec4(Position.x, Position.y, Position.z, 1.0);

	// Position of the vertex, in worldspace 
	Position_worldspace = ModelMatrix * vec4(Position,1);
	
	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	vec4 vertexPosition_cameraspace = ViewMatrix * ModelMatrix * vec4(Position,1);
	EyeDirection_cameraspace = vec4(0,0,0,1.0) - vertexPosition_cameraspace;
	//EyeDirection_cameraspace = vec3(0,0,0) - (ModelMatrix * vec4(Position, 1)).xyz;

	// Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
	vec4 LightPosition_cameraspace = ViewMatrix * vec4(LightPosition_worldspace,1);
	//vec3 LightPosition_cameraspace = LightPosition_worldspace;
	LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;
	
	// Normal of the the vertex, in camera space
	Normal_cameraspace = inverse(transpose(ViewMatrix * ModelMatrix)) * vec4(vertexNormal,0); // Use  inverse transpose because of non-uniform-scaling 
	//Normal_cameraspace = ( inverse(transpose(ModelMatrix)) * vec4(vertexNormal,0)).xyz;
   	vColor = Color;
}
