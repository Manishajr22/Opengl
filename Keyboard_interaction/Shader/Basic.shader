#shader vertex
#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 aNormal;

uniform mat4 mvp;
out vec3 FragPos;
out vec3 Normal;

void main()
{
	gl_Position = mvp* position;
	FragPos = vec3(position);
	Normal = aNormal;

}

#shader fragment
#version 330 core
in vec3 Normal;
in vec3 FragPos;
layout(location = 0) out vec4 u_Color;

uniform  vec3 position;
//uniform vec3 viewPos;
void main()

{	//phong lighting
	//vec3 lightcolor = vec3(1.0f, 1.0f, 1.0f);
	float specularStrength = 0.5;
	vec3 m_color = vec3(0.0, 0.2, 0.8);
	vec3 lightpos = vec3(2.0f, 2.0f, 2.0f);
	vec3 viewPos = vec3(-1.0f, -1.0f, 1.0f);
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightpos - vec3(vec4(position, 1.0)));
	float diff = max(dot(norm, lightDir), 0.3);		
	vec3 diffuse = diff * vec3(1.0f,1.0f,1.0f);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec *vec3(1.0f, 1.0f, 1.0f);
	u_Color = vec4((diffuse+ specular)*m_color, 0.3);
}