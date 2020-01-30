// #VERTEX#

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

out PointLight tangent_point_light;

void main()
{
    gl_Position = MVP * vec4(vPos, 1.0);
    position = (M * vec4(vPos, 1.0)).xyz;
    uv = vUV * .5f;

    mat3 ITM = inverse(transpose(mat3(M)));
    vec3 T = normalize(ITM * vT);
    vec3 N = normalize(ITM * vN);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    mat3 TBN = transpose(mat3(T, B, N));

    tangent_point_light.pos = TBN * point_light.pos;
    tangent_point_light.color = point_light.color;
    tangent_camera_pos = TBN * EYE;
    tangent_position = TBN * position;
};

// #FRAGMENT#

in vec3 position;
in vec3 tangent_position;
in vec2 uv;
in vec3 tangent_camera_pos;

struct PointLight {
    vec3 pos;
    vec3 color;
};
in PointLight tangent_point_light;

out vec4 output_color;

uniform sampler2D diff_tex;
uniform sampler2D spec_tex;
uniform sampler2D bump_tex;

uniform vec3 tint;
uniform float specularity;
uniform float bump_strength;

void main()
{
    vec3 normal = texture(bump_tex, uv).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    normal = mix(vec3(0.f, 0.f, 1.f), normal, bump_strength);

    vec3 object_color = texture(diff_tex, uv).rgb;
    vec3 object_specular = texture(spec_tex, uv).rgb;

    vec3 ambient = 0.1 * object_color;

    vec3 pos_to_light = normalize(tangent_point_light.pos - tangent_position);
    float diff = max(dot(pos_to_light, normal), 0.0f);
    vec3 diffuse = diff * object_color * tangent_point_light.color * tint;

    vec3 pos_to_camera = normalize(tangent_camera_pos - tangent_position);
    float spec = clamp(dot(reflect(-pos_to_light, normal), pos_to_camera), 0.0f, 1.0f);
    vec3 specular = specularity * object_specular * pow(spec, 20.0f) * tangent_point_light.color;

    output_color = vec4(ambient + diffuse + specular, 1.0);
};