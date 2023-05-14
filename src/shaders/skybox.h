#ifndef SKYBOX_H
#define SKYBOX_H

#include "../renderer/gl.h"
#include "../renderer/view.h"
#include "../renderer/scene.h"
#include "../renderer/vertex_buffer.h"
#include "../common/nui_math.h"
#include <stdbool.h>

typedef struct {
  GLuint    texture;
  GLuint    shader;
  mesh_t    mesh;
} skybox_t;

bool skybox_init(skybox_t *skybox, vertex_buffer_t *vertex_buffer);
void skybox_render(skybox_t *skybox, view_t *view, quat_t view_angle);

#endif