#version 330

uniform vec4 colorUniform = vec4(0.5, 0.5, 0.5, 1.0);
uniform int getColor = 0;

in vec4 vColor;

out vec4 FragColor;

void main()
{
    if(getColor == 0) {
        FragColor = vColor;
        } else {
        FragColor = colorUniform;
        }
}