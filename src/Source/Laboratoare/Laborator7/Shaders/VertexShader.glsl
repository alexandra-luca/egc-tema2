#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Uniforms for light properties
uniform vec3 light_position;
uniform vec3 eye_position;
uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform vec3 object_color;

// Output value to fragment shader
out vec3 color;

void main()
{
	// TODO: compute world space vectors
	vec3 world_position = vec3 (Model * vec4 (v_position, 1));
	vec3 world_normal = normalize(vec3 (Model * vec4 (v_normal, 0)));
	
	vec3 L = normalize(light_position - world_position);
	vec3 V = normalize(eye_position - world_position);
	vec3 R = reflect (-L, world_normal);

	// TODO: define ambient light component
	float ambient_light = material_kd * 0.25;

	// TODO: compute diffuse light component
	float diffuse_light = material_kd * 0.2 * max (dot(world_normal,L), 0);

	// TODO: compute specular light component
	float specular_light = 0;

	if (diffuse_light > 0)
	{
		specular_light = 0.25;// material_ks * 0.2 * pow(max(dot(V, R), 0), 2);
	}

	// TODO: compute light
	float d = length(light_position - world_position);
	float intensitate = ambient_light + (1/pow(d, 2)) * (diffuse_light + specular_light);

	// TODO: send color light output to fragment shader
	color = object_color * intensitate;

	gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
