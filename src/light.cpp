#include <light.h>

#include <materials.h>
#include <mesh_io.h>
#include <mesh.h>

PointLight point_lights[N_POINT_LIGHTS];

static LightMaterial light_mat;
static uint32_t light_mesh;

void lights_init()
{
    material_create(light_mat);

    mesh_create(&light_mesh, "../resources/meshes/sphere.obj");

    VertexAttribs attribs;
    vertex_attribs_append(attribs, 3, GL_FLOAT);
    vertex_attribs_append(attribs, 2, GL_FLOAT);
    vertex_attribs_append(attribs, 3, GL_FLOAT);
    vertex_attribs_append(attribs, 3, GL_FLOAT);
    vertex_attribs_append(attribs, 3, GL_FLOAT);
    vertex_attribs_enable_all(attribs, light_mesh);
}
void lights_draw(const glm::mat4& vp)
{
    for (uint32_t i = 0; i < N_POINT_LIGHTS; ++i)
    {
        if (glm::length2(point_lights[i].color) < 0.001f)
            continue;
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, point_lights[i].pos);
        model = glm::scale(model, glm::vec3(.1f));
        light_mat.light_index = i;
        material_use(light_mat, vp * model, glm::mat4(0.f), glm::vec3(0.f));
        mesh_draw(light_mesh);
    }
}
