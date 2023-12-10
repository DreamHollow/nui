#include "renderer.h"

#include "gl.h"

#define SCR_WIDTH 1280
#define SCR_HEIGHT 720

#define VIEW_SCALE 4

#define VIEW_WIDTH (SCR_WIDTH / VIEW_SCALE)
#define VIEW_HEIGHT (SCR_HEIGHT / VIEW_SCALE)

#include "light.h"
#include "camera.h"
#include "model.h"
#include "flat.h"
#include "material.h"
#include "renderer_api.h"
#include "shader.h"
#include "mesh.h"
#include "frame.h"
#include "hdr.h"
#include "blur.h"
#include "quad.h"
#include "dither.h"
#include "defer.h"

typedef struct {
  view_t view;
  
  model_t fumo_model;
  model_t map_model;
  
  GLuint test_shader;
  GLuint ul_view_pos;
  
  const game_t *game;
} renderer_t;

static renderer_t renderer;

static void renderer_init_gl();
static void renderer_init_scene();
static void renderer_scene_render();

static bool setup_defer();

bool renderer_init(const game_t *game)
{
  renderer_init_gl();
  mesh_buffer_init(1024 * 1024);
  
  if (!quad_init()) {
    return false;
  }
  
  if (!flat_init()) {
    return false;
  }
  
  if (!light_init()) {
    return false;
  }
  
  if (!hdr_init(VIEW_WIDTH, VIEW_HEIGHT)) {
    return false;
  }
  
  if (!blur_init(VIEW_WIDTH, VIEW_HEIGHT)) {
    return false;
  }
  
  if (!dither_init(VIEW_WIDTH, VIEW_HEIGHT)) {
    return false;
  }
  
  if (!defer_init(VIEW_WIDTH, VIEW_HEIGHT)) {
    return false;
  }
  
  if (!setup_defer()) {
    return false;
  }
  
  camera_init();
  
  if (!model_load(&renderer.fumo_model, "cirno_fumo")) {
    return false;
  }
  
  renderer.game = game;
  
  float aspect_ratio = (float) SCR_HEIGHT/ (float) SCR_WIDTH;
  view_set_perspective(&renderer.view, aspect_ratio, to_radians(90.0), 0.1, 100.0);
  
  return true;
}

static bool setup_defer()
{
  if (!shader_load(&renderer.test_shader, "test", "")) {
    return false;
  }
  
  glUseProgram(renderer.test_shader);
  
  renderer.ul_view_pos = glGetUniformLocation(renderer.test_shader, "u_view_pos");
  
  GLuint ul_pos = glGetUniformLocation(renderer.test_shader, "u_pos");
  GLuint ul_normal = glGetUniformLocation(renderer.test_shader, "u_normal");
  GLuint ul_albedo = glGetUniformLocation(renderer.test_shader, "u_albedo");
  
  glUniform1i(ul_pos, 0);
  glUniform1i(ul_normal, 1);
  glUniform1i(ul_albedo, 2);
  
  return true;
}

static void renderer_init_scene()
{
  light_sub_point(0, vec3_init( 3.0, 5.0,  3.0), 30.0, vec4_init(0.0, 1.0, 1.0, 1.0));
  light_sub_point(1, vec3_init(-3.0, 5.0, -3.0), 30.0, vec4_init(1.0, 0.0, 1.0, 1.0));
}

static void renderer_init_gl()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_SCISSOR_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void renderer_render()
{
  /*
  defer_begin();
  camera_set_view(renderer.view);
  camera_move(renderer.game->player.position, renderer.game->player.rotation);
  renderer_scene_pass();
  defer_end();
  
  dither_begin();
  defer_bind();
  glUseProgram(renderer.test_shader);
  
  vec3_t view_pos = renderer.game->player.position;
  glUniform3f(renderer.ul_view_pos, view_pos.x, view_pos.y, view_pos.z);
  
  quad_draw();
  
  dither_end();
  
  dither_draw(0, 0, SCR_WIDTH, SCR_HEIGHT);
  */
  
  hdr_begin();
  camera_set_view(renderer.view);
  renderer_scene_render();
  hdr_end();
  
  dither_begin();
  hdr_draw();
  dither_end();
  
  camera_set_viewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  dither_draw();
}

void renderer_scene_render()
{
  camera_move(renderer.game->player.position, renderer.game->player.rotation);
  
  light_bind();
  light_sub_view_pos(renderer.game->player.position);
  
  renderer_scene_pass();
}

void renderer_scene_pass()
{
  camera_model(mat4x4_init_identity());
  model_draw(&renderer.fumo_model);
  model_draw(&renderer.map_model);
}

void renderer_shadow_pass()
{
  glClear(GL_DEPTH_BUFFER_BIT);
  
  camera_model(mat4x4_init_identity());
  
  model_draw(&renderer.fumo_model);
  model_draw(&renderer.map_model);
}

void renderer_map_load(const map_t *map)
{
  model_load_map(&renderer.map_model, map);
  renderer_init_scene();
}
