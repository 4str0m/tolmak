
// #VERTEX#

#version 460

uniform mat4 MVP;
uniform mat4 M;

in vec3 vPos;
in vec2 vUV;
in vec3 vN;

out vec3 normal;
out vec3 position;

void main()
{
    gl_Position = MVP * vec4(vPos, 1.0);
	mat3 ITM = mat3(inverse(transpose(M)));
	normal = ITM * vN;
	position = (M * vec4(vPos, 1.0)).xyz;
};

// #FRAGMENT#

#version 460

in vec3 normal;
in vec3 position;

const vec3 light_color = vec3(0.8f, 0.8f, 0.7f);

void main()
{
	vec3 norm = normalize(normal);
	vec3 pos_to_light = normalize(vec3(0, 0, 4) - position);
    float specular = clamp(dot(reflect(-pos_to_light, norm), pos_to_light), 0.0f, 1.0f);
    vec3 object_color = vec3(1.f);
    vec3 color = vec3(0.f);
    color += (0.2f + max(dot(pos_to_light, norm), 0.0f)) * object_color * light_color;
    color += pow(specular, 20.0f) * light_color;

//     gl_FragColor = vec4(abs(normal), 1.0);
    gl_FragColor = vec4(color, 1.0);
};