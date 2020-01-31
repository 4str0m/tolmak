// #VERTEX#

layout (location = 0) in vec3 vPos;
out vec3 position;
out vec3 light_color;

void main()
{
    gl_Position = MVP * vec4(vPos, 1.0);
};

// #FRAGMENT#

out vec4 output_color;

%Hide()
uniform int light_index;

void main()
{
    output_color = vec4(point_lights[light_index].color, 1.0);
};