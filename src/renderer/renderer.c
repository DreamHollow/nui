#include "renderer.h"

#include "gl.h"
#include "../piplines/main_pipeline.h"

static bool renderer_init_primitive_mesh(renderer_t *renderer);

bool renderer_init(renderer_t *renderer)
{
  buffer_init(&renderer->buffer, 4096);
  
  view_init(&renderer->view);
  renderer->view.projection_matrix = mat4x4_init_perspective(720.0 / 1280.0, to_radians(90.0), 0.1, 100.0);
  
  renderer_init_primitive_mesh(renderer);
  
  if (!lights_init(&renderer->lights))
    return false;
  
  if (!hdr_init(&renderer->hdr, renderer->quad_mesh))
    return false;
  
  if (!waves_init(&renderer->waves, renderer->quad_mesh))
    return false;
  
  if (!skybox_init(&renderer->skybox, &renderer->buffer))
    return false;
  
  renderer->pipeline = (pipeline_t) {
    .buffer = &renderer->buffer,
    .hdr    = &renderer->hdr,
    .lights = &renderer->lights,
    .skybox = &renderer->skybox,
    .init_scene = main_pipeline_init_scene,
    .render_scene = main_pipeline_render_scene
  };
  
  if (!renderer->pipeline.init_scene(&renderer->pipeline, &renderer->scene, &renderer->view))
    return false;
  
  return true;
}

void renderer_render(renderer_t *renderer, const game_t *game)
{
  renderer->pipeline.render_scene(&renderer->pipeline, &renderer->scene, game, &renderer->view);
}

static bool renderer_init_primitive_mesh(renderer_t *renderer)
{
  vertex_t quad_vertices[] = {
    { .pos = { -1.0f, -1.0f, 0.0f }, .uv = { 0.0f, 0.0f } },
    { .pos = { -1.0f, +1.0f, 0.0f }, .uv = { 0.0f, 1.0f } },
    { .pos = { +1.0f, -1.0f, 0.0f }, .uv = { 1.0f, 0.0f } },
    { .pos = { -1.0f, +1.0f, 0.0f }, .uv = { 0.0f, 1.0f } },
    { .pos = { +1.0f, +1.0f, 0.0f }, .uv = { 1.0f, 1.0f } },
    { .pos = { +1.0f, -1.0f, 0.0f }, .uv = { 1.0f, 0.0f } }
  };

  if (!buffer_new_mesh(
    &renderer->buffer,
    &renderer->quad_mesh,
    quad_vertices,
    6
  )) {
    return false;
  }
  
  return true;
}
