#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 rgb;

void main()
{
	FragColor = vec4(rgb, 1.0);
	//FragColor = vec4(0.9, 0.8, 0.7, 1.0);
}
