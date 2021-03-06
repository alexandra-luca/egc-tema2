#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform int is_powerup;

// Output values to fragment shader
out vec3 world_position;
out vec3 world_normal;
out vec3 normal;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	
	vec3 newpos = v_position;
	if (is_powerup == 1)
		newpos = v_position + v_normal * (rand(vec2(v_position)) * 0.5);

	// TODO: compute world space vertex position and normal
	// TODO: send world position and world normal to Fragment Shader	
	world_position = vec3 (Model * vec4 (newpos, 1));
	world_normal = normalize(vec3 (Model * vec4 (v_normal, 0)));
	normal = v_normal;

	gl_Position = Projection * View * Model * vec4(newpos, 1.0);
}
