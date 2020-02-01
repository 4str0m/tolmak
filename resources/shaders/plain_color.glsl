// #VERTEX#
layout (location = 0) in vec3 vPos;

void main()
{
    gl_Position = MVP * vec4(vPos, 1.0);
};

// #FRAGMENT#
#pragma optionNV unroll all

out vec4 output_color;

%Color()
uniform vec3 tint;

void main()
{
    output_color = vec4(tint, 1.0);
};