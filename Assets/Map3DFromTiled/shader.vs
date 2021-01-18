#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in mat4 aModelMatrix;
//layout (location = 9) in mat3 aNormalTransform;

uniform mat4 uCameraMatrix;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;


void main()
{	
	gl_Position = uCameraMatrix * aModelMatrix * vec4(aPos, 1.0);
		
	//gl_Position.y += wave_amplitude * sin(wave_frequency * t + gl_Position.x);
	
	TexCoord = aTexCoord;
	Normal = aNormal;
}
