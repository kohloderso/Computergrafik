#version 330

out vec4 outputColor;

uniform mat4 ViewMatrix;

in vec4  worldPosition;
in vec3  interpNormal;
in vec3  vColor;

void main()
{
   vec3 lightDirection;
   vec3 reflectDirection;
   float cosAngIncidence;
   float phongTerm;

   vec3 lightPosition = vec3(100.0f, 100.0f, 100.0f);
   vec4 diffuseColor = vec4(vColor, 1.0f);
float ambientIntensity = 0.5;
   float lightIntensity = 1;
float shininessFactor = 1;

   //Initialize the output color
   outputColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);

   //Calculate the view direction in world space
   vec3 viewDirection = normalize(vec3(inverse(ViewMatrix) * vec4(0.0, 0.0, 0.0, 1.0) - worldPosition));

   
        // Set ambient light if enabled
        outputColor = outputColor + ambientIntensity * diffuseColor;

        // Calculate diffuse light
        lightDirection = normalize(lightPosition - vec3(worldPosition));

        //Interpolated normals are no longer unit vectors
        cosAngIncidence = dot(normalize(interpNormal), lightDirection);
        cosAngIncidence = clamp(cosAngIncidence, 0, 1);

        outputColor = outputColor + lightIntensity * diffuseColor * cosAngIncidence;

        // Calculate specular light
        
    // reflect expects vector from light
    reflectDirection = reflect(-lightDirection, normalize(interpNormal));

    phongTerm = dot(viewDirection, reflectDirection);
    phongTerm = clamp(phongTerm, 0, 1);
    phongTerm = pow(phongTerm, shininessFactor);

    outputColor = outputColor + lightIntensity * phongTerm;

}
