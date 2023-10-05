#include <stdio.h>
#include <stdbool.h>
#include "sdl.h"
#include "common/log.h"
#include "game/game.h"
#include "game/map.h"
#include "renderer/gl.h"
#include "renderer/renderer.h"

#ifdef __EMSCRIPTEN__
  #include <emscripten.h>
#endif

static struct {
  sdl_t       sdl;
  renderer_t  renderer;
  game_t      game;
  usercmd_t   usercmd;
} nui;

bool nui_init();
void nui_update();

int main()
{
  if (!nui_init())
    return 1;
  
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(nui_update, 0, true);
#else
  while (!nui.sdl.quit)
    nui_update();
#endif
  
  sdl_quit(&nui.sdl);
  
  return 0;
}

bool nui_init()
{
  if (!sdl_init(&nui.sdl, 1280, 720, "nui"))
    return false;
  
  if (!gl_init())
    return false;
  
  nui.usercmd = (usercmd_t) {0};
  game_init(&nui.game);
  sdl_lock(true);
  
  if (!renderer_init(&nui.renderer, &nui.game)) {
    return false;
  }
  
  map_t *map = map_load("nk_construct");
  
  game_map_load(&nui.game, map);
  renderer_map_load(&nui.renderer, map);
  
  map_free(map);
  
  return true;
}

void nui_update()
{
  sdl_poll(&nui.sdl, &nui.usercmd);
  game_update(&nui.game, &nui.usercmd);
  renderer_render(&nui.renderer);
  sdl_swap(&nui.sdl);
}
