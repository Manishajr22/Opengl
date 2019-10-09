#shader vertex
#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
uniform mat4 model;
uniform vec3 view;
out vec3 FragPos;
out vec3 vray_dir;
out vec3 eye;
out vec2 v_TexCoord;
void main()
{
	gl_Position = model * position;
	FragPos = vec3(position);
	
	
	eye = view;
	vray_dir = eye- FragPos ;
	v_TexCoord = texCoord;
}


#shader fragment
#version 330 core
in vec3 eye;
in vec3 vray_dir;
layout(location = 0) out vec4 u_Color;
uniform sampler2D colormap;
uniform sampler3D u_Texture;
uniform vec3 volume_dims;

vec2 intersect_box(vec3 orig, vec3 dir)
{
	 vec3 box_min = vec3(0);
	 vec3 box_max = vec3(1);
	vec3 inv_dir = 1.0 / dir;
	vec3 tmin_tmp = (box_min - orig) * inv_dir;
	vec3 tmax_tmp = (box_max - orig) * inv_dir;
	vec3 tmin = min(tmin_tmp, tmax_tmp);
	vec3 tmax = max(tmin_tmp, tmax_tmp);

	float t0 = max(tmin.x, max(tmin.y, tmin.z));
	float t1 = min(tmax.x, min(tmax.y, tmax.z));
	return vec2(t0, t1);
}

void main()
{

	// Step 1: Normalize the view ray
	vec3 ray_dir = normalize(vray_dir);

	// Step 2: Intersect the ray with the volume bounds to find the interval
	// along the ray overlapped by the volume.
	vec2 t_hit = intersect_box(eye, ray_dir);
	if (t_hit.x > t_hit.y) {
		discard;
	}
	// We don't want to sample voxels behind the eye if it's
	// inside the volume, so keep the starting point at or in front
	// of the eye
	t_hit.x = max(t_hit.x, 0.0);

	// Step 3: Compute the step size to march through the volume grid
	vec3 dt_vec =(vec3(128) * abs(ray_dir));
	float dt = min(dt_vec.x, min(dt_vec.y, dt_vec.z));
	// Step 4: Starting from the entry point, march the ray through the volume
	// and sample it
	vec3 p = eye + ray_dir * ( t_hit.x);
	for (float t = t_hit.x; t <= t_hit.y; t += dt) {
		// Step 4.1: Sample the volume, and color it by the transfer function.
		// Note that here we don't use the opacity from the transfer function,
		// and just use the sample value as the opacity
		float val = texture(u_Texture, p).r;
		vec4 val_color = vec4(texture(colormap, vec2(val, 0.5)).rgb, val);

		// Step 4.2: Accumulate the color and opacity using the front-to-back
		// compositing equation
		u_Color.rgb += val_color.rgb;
		u_Color.a +=  val_color.a;
		
		// Optimization: break out of the loop when the color is near opaque
		if (u_Color.a >= 0.95 ){
			break;
		}
		p += ray_dir * dt ;
	}


}
