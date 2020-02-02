// #VERTEX#
#pragma optionNV unroll all

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vUV;
layout (location = 2) in vec3 vN;
layout (location = 3) in vec3 vT;
layout (location = 4) in vec3 vB;

out vec3 position;
out vec3 tangent_position;
out vec2 uv;
out vec2 normal;
out vec3 tangent_camera_pos;

out PointLight tangent_point_lights[N_POINT_LIGHTS];

%Range(0.1, 5.0, 1.0)
uniform float uv_scale;

void main()
{
    gl_Position = MVP * vec4(vPos, 1.0);
    position = (M * vec4(vPos, 1.0)).xyz;
    uv = vUV * uv_scale;

    mat3 ITM = inverse(transpose(mat3(M)));
    vec3 T = normalize(ITM * vT);
    vec3 N = normalize(ITM * vN);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(T, N);
    mat3 TBN = transpose(mat3(T, B, N));

    for(int i = 0; i < N_POINT_LIGHTS; ++i) {
        tangent_point_lights[i].pos = TBN * point_lights[i].pos;
        tangent_point_lights[i].color = point_lights[i].color;
    }
    tangent_camera_pos = TBN * EYE;
    tangent_position = TBN * position;
};

// #FRAGMENT#
#pragma optionNV unroll all

in vec3 position;
in vec3 tangent_position;
in vec2 uv;
in vec3 tangent_camera_pos;

in PointLight tangent_point_lights[N_POINT_LIGHTS];

out vec4 output_color;

uniform sampler2D diff_tex;
uniform sampler2D spec_tex;
uniform sampler2D bump_tex;

%Color(1.0, 1.0, 1.0)
uniform vec3 tint;

%Range(0.0, 1.0, 1.0)
uniform float specularity;
%Range(0.0, 1.0, 0.5)
uniform float bump_strength;

vec3 point_light(PointLight point_light, vec3 normal, vec3 object_color, float object_specular)
{
    vec3 pos_to_light = normalize(point_light.pos - tangent_position);
    float diff = max(dot(pos_to_light, normal), 0.0f);
    vec3 diffuse = diff * object_color * point_light.color * tint;

    vec3 pos_to_camera = normalize(tangent_camera_pos - tangent_position);
    float spec = clamp(dot(reflect(-pos_to_light, normal), pos_to_camera), 0.0f, 1.0f);
    vec3 specular = specularity * object_specular * pow(spec, 20.0f) * point_light.color;
    return diffuse + specular;
}

void main()
{
    vec3 normal = texture(bump_tex, uv).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    normal = mix(vec3(0.f, 0.f, 1.f), normal, bump_strength);

    vec3 object_color = texture(diff_tex, uv).rgb;
    float object_specular = texture(spec_tex, uv).r;

    vec3 ambient = 0.1 * object_color;

    vec3 color = ambient;
    for(unsigned int i = 0; i < N_POINT_LIGHTS; ++i)
        color += point_light(tangent_point_lights[i], normal, object_color, object_specular);

    output_color = vec4(color, 1.0);
};