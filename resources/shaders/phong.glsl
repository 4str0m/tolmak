
// #VERTEX#

#version 460

uniform mat4 MVP;
uniform mat4 M;
uniform vec3 EYE;

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vUV;
layout (location = 2) in vec3 vN;

out vec3 normal;
out vec3 position;
out vec2 uv;
out vec3 camera_pos;

void main()
{
    gl_Position = MVP * vec4(vPos, 1.0);
	mat3 ITM = mat3(inverse(transpose(M)));
	normal = ITM * vN;
	position = (M * vec4(vPos, 1.0)).xyz;
	uv = vUV;
	camera_pos = EYE;
};

// #FRAGMENT#

#version 460

in vec3 normal;
in vec3 position;
in vec2 uv;
in vec3 camera_pos;

out vec4 output_color;

uniform sampler2D diff_tex;

const vec3 light_color = vec3(0.8f, 0.8f, 0.7f);

void main()
{
	vec3 norm = normalize(normal);
	vec3 pos_to_light = normalize(vec3(0, 0, 4) - position);
	vec3 pos_to_camera = normalize(camera_pos - position);
    float specular = clamp(dot(reflect(-pos_to_light, norm), pos_to_camera), 0.0f, 1.0f);
    vec3 object_color = texture(diff_tex, uv).rgb;
    vec3 color = vec3(0.f);
    color += (0.2f + max(dot(pos_to_light, norm), 0.0f)) * object_color * light_color;
    color += pow(specular, 20.0f) * light_color;

    output_color = vec4(color, 1.0);
};