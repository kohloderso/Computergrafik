#version 330

const int NUMLIGHTS = 2;

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


void main()
{
   	gl_Position = ProjectionMatrix*ViewMatrix*ModelMatrix*vec4(Position.x, Position.y, Position.z, 1.0);

	// Position of the vertex, in worldspace 
	Position_worldspace = ModelMatrix * vec4(Position,1);
	
	EyeDirection_worldspace = vec4(0,0,0, 1.0) - Position_worldspace;

	
	// Normal of the the vertex, in world space
	Normal_worldspace = InverseTransposeModel * vec4(vertexNormal, 0.0f);
   	vColor = Color;
}
