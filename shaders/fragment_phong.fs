#version 330
#pragma optimize (off)
#define NUMLIGHTS 7

struct DirectionalLight
{
    vec3 color;
    vec3 position;
    float diffuseIntensity;
    bool useAmbient;
    bool useDiffuse;
    bool useSpecular;
};

uniform mat4 ViewMatrix;
uniform DirectionalLight lightsources[NUMLIGHTS];

in vec4  worldPosition;
in vec3  interpNormal;
in vec3  vColor;

out vec4 outputColor;

void main()
{
   vec3 lightDirection;
   vec3 reflectDirection;
   float cosAngIncidence;
   float phongTerm;

   float ambientIntensity = 0.4;
   float shininessFactor = 1;

   //Initialize the output color
   outputColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);

   //Calculate the view direction in world space
   vec3 viewDirection = normalize(vec3(/*inverse(ViewMatrix)*/vec4(0.0, 0.0, 0.0, 1.0) - worldPosition));

   // ambient light
   outputColor = outputColor + ambientIntensity * vec4(vColor,1.0f);
   
   for(int i = 0; i < NUMLIGHTS; i++) {
    	// diffuse light
        lightDirection = normalize(lightsources[i].position - vec3(worldPosition));

        //Interpolated normals are no longer unit vectors
        cosAngIncidence = dot(normalize(interpNormal), lightDirection);
        cosAngIncidence = clamp(cosAngIncidence, 0, 1);

        outputColor = outputColor + lightsources[i].diffuseIntensity * (lightsources[i].color, 1.0f) * cosAngIncidence;

        // specular light
        
       	// reflect expects vector from light
     	reflectDirection = reflect(-lightDirection, normalize(interpNormal));

    	phongTerm = dot(viewDirection, reflectDirection);
    	phongTerm = clamp(phongTerm, 0, 1);
    	phongTerm = pow(phongTerm, shininessFactor);

    	outputColor = outputColor + phongTerm * vec4(lightsources[i].color, 1.0f);
   }

}
