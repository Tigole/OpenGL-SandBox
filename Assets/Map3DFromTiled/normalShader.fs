#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;

uniform float t;
uniform sampler2D texture1;

void main()
{
	vec3 Color = vec3(0.3, 0.7, 0.5);
	
	Color = vec3(0.9, 0.0, 0.1);

	FragColor = texture(texture1, TexCoord);
	//FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
	FragColor = vec4(0.5, 0.3, 0.7, 1.0);
	
	//FragColor = vec4(0.1, 0.64, 0.93, 0.6);
}
