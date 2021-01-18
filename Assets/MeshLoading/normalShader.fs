#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform float t;
uniform sampler2D texture1;

void main()
{
	FragColor = texture(texture1, TexCoord);
	
	//FragColor = vec4(FragPos.z, FragPos.z, FragPos.z, 1.0);
	//FragColor = vec4(FragPos, 1.0);
	//FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
	//FragColor = vec4(0.5, 0.3, 0.7, 1.0);
	
	//FragColor = vec4(0.1, 0.64, 0.93, 0.6);
}
