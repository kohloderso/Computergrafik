#version 330

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat4 InverseTransposeModel;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;
layout (location = 2) in vec3 Normal;

out vec4 worldPosition;
out vec3 interpNormal;
out vec3 vColor;

void main()
{
   // Calculate world and camera position of the vertex
   worldPosition = ModelMatrix * vec4(Position, 1.0);
   gl_Position = ProjectionMatrix * ViewMatrix * worldPosition;

    // Pass normals in world space interpolated to the fragment shader
   interpNormal = normalize(vec3(InverseTransposeModel * vec4(Normal, 0.0)));

   vColor = Color;

}
