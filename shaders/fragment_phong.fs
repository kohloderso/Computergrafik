#version 330
#pragma optimize (off) // otherwise struct not working
#define NUMLIGHTS 2

struct DirectionalLight
{
	vec3 color;
	vec3 position;
	float intensity;
}

out vec4 outputColor;

uniform mat4 ViewMatrix;
uniform DirectionalLight lightsources[NUMLIGHTS];
uniform bool AmbientOn = true;
uniform bool SpecularOn = true;
uniform bool DiffuseOn = true;

in vec4 worldPosition;
in vec3 interpNormal;
in vec3 vColor;

void main() {

	DirectionalLight test;
	test.color = vec3(1.0f, 1.0f, 1.0f);
	test.position = vec3(2.0f, 5.0f, 0.0f);
	test.intensity = 5;
	vec3 lightDirection;
	vec3 reflectDirection;
	float cosAngIncidence;
	float phongTerm;

	float ambientIntensity = 0.4;
	float shininessFactor = 2.0;

	//Initialize the output color
	outputColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);

	//Calculate the view direction in world space
	vec3 viewDirection = normalize(vec3(vec4(0.0, 0.0, 0.0, 1.0) - worldPosition));
	
	// ambient light
	outputColor = outputColor + ambientIntensity * vec4(vColor, 1.0f);
	
	for(int i = 0; i < 1; i++) {
	
		// diffuse light
		lightDirection = normalize(test.position - vec3(worldPosition));
		cosAngIncidence = dot(normalize(interpNormal), lightDirection);
		cosAngIncidence = clamp(cosAngIncidence, 0, 1);
		outputColor = outputColor + test.intensity * vec4(test.color, 1.0f) * cosAngIncidence;
	
		// specular light
		// reflect expects vector from light, therefore -lightDirection
		reflectDirection = reflect(-lightDirection, normalize(interpNormal));
		phongTerm = dot(viewDirection, reflectDirection);
		phongTerm = clamp(phongTerm, 0, 1);
		phongTerm = pow(phongTerm, shininessFactor);
		outputColor = outputColor + vec4(test.color, 1.0f) * phongTerm;
	}
//outputColor = vec4(vColor, 10.0f);
}
