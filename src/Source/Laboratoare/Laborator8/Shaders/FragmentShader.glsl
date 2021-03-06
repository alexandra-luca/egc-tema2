#version 330

// TODO: get color value from vertex shader
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform vec3 object_color;

uniform int is_spotlight;

layout(location = 0) out vec4 out_color;

void main()
{
	vec3 L = normalize(light_position - world_position);
	vec3 V = normalize(eye_position - world_position);
	vec3 R = reflect (-L, world_normal);

	// TODO: define ambient light component
	float ambient_light = material_kd * 0.25;

	// TODO: compute diffuse light component
	float diffuse_light = 0;
	
	if (is_spotlight == 0) {
		diffuse_light = material_kd * 0.2 * max (dot(world_normal,L), 0);
	} else {
		float cut_off = radians(30);
		float spot_light = dot(-L, light_direction);
		float spot_light_limit = cos(cut_off);

		if (spot_light > cos(cut_off))
		{
			float linear_att = (spot_light - spot_light_limit) / (1.0f - spot_light_limit);
			float light_att_factor = pow(linear_att, 2);

			diffuse_light = material_kd * 0.2 * max (dot(world_normal,L), 0) * light_att_factor;
		}
	}

	// TODO: compute specular light component
	float specular_light = 0;

	if (diffuse_light > 0)
	{
		specular_light = 0.25;// material_ks * 0.2 * pow(max(dot(V, R), 0), 2);
	}

	// TODO: compute light
	float d = length(light_position - world_position);
	float intensitate = ambient_light + (1/pow(d, 2)) * (diffuse_light + specular_light);

	// TODO: write pixel out color
	vec3 color = object_color * intensitate;
	out_color = vec4(color[0], color[1], color[2], 1);
}