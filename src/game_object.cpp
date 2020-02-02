#include <game_object.h>

static uint32_t UID = 1;

void game_object_create(GameObject& game_object, const char* obj_file_path)
{
    game_object.uid = UID++;

    mesh_create(&game_object.mesh_id, obj_file_path);

    VertexAttribs attribs;
    vertex_attribs_append(attribs, 3, GL_FLOAT);
    vertex_attribs_append(attribs, 2, GL_FLOAT);
    vertex_attribs_append(attribs, 3, GL_FLOAT);
    vertex_attribs_append(attribs, 3, GL_FLOAT);
    vertex_attribs_append(attribs, 3, GL_FLOAT);
    vertex_attribs_enable_all(attribs, game_object.mesh_id);
}
