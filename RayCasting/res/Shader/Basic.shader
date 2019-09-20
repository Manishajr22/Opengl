#shader vertex
#version 330 core
layout(location = 0) in vec4 position;
uniform mat4 model;
uniform vec3 view;
out vec3 FragPos;
out vec3 vray_dir;
out vec3 eye;

void main()
{
	gl_Position = model * position;
	FragPos = vec3(position);
	vray_dir = vec3(view)-vec3(position);
	eye = view;
}

#shader fragment
#version 330 core
in vec3 FragPos;
in vec3 eye;
in vec3 vray_dir;
layout(location = 0) out vec4 u_Color;
uniform sampler2D colormap;
void main()
{	
	vec3 viewDir = normalize(FragPos- eye);
	vec3 p = eye + vray_dir;
	u_Color = vec4(texture(colormap, vec2(p)).rgb, 0.5);
}