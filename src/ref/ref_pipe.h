#ifndef REF_PIPE_H
#define REF_PIPE_H

#include "ref_def.h"
#include "view.h"
#include "../shaders/skybox.h"
#include "../shaders/lights.h"
#include "../game/game.h"

typedef struct ref_pipe {
  buffer_t  *buffer;
  skybox_t  *skybox;
  lights_t  *lights;
  
  bool (*init_scene)(struct ref_pipe *ref_pipe, scene_t *scene, view_t *view);
  void (*render_scene)(struct ref_pipe *ref_pipe, const scene_t *scene, const game_t *game, view_t *view);
} ref_pipe_t;

#endif
