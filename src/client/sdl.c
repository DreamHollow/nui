#include "sdl.h"

#include <stdbool.h>
#include "../common/log.h"
#include "../ngui/ngui.h"
#include <SDL2/SDL.h>

typedef struct {
  SDL_Window *window;
  SDL_GLContext gl_context;
  
  bool focus_gui;
  bool focus_game;
  
  bool quit;
} sdl_t;

static sdl_t sdl;

static void key_event(usercmd_t *usercmd, int key, int action);
static void mouse_event(usercmd_t *usercmd, int button, int action);
static void mouse_move(usercmd_t *usercmd, int d_x, int d_y);
static void text_input_event(const char *text);

void sdl_lock(bool set_lock)
{
  SDL_SetRelativeMouseMode(set_lock);
}

void sdl_poll(usercmd_t *usercmd)
{
  usercmd->d_yaw = 0;
  usercmd->d_pitch = 0;
  
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      sdl.quit = true;
      break;
    case SDL_TEXTINPUT:
      text_input_event(event.text.text);
      break;
    case SDL_KEYUP:
      key_event(usercmd, event.key.keysym.sym, 0);
      break;
    case SDL_KEYDOWN:
      key_event(usercmd, event.key.keysym.sym, 1);
      break;
    case SDL_MOUSEBUTTONDOWN:
      mouse_event(usercmd, event.button.button, 1);
      break;
    case SDL_MOUSEBUTTONUP:
      mouse_event(usercmd, event.button.button, 0);
      break;
    case SDL_MOUSEMOTION:
      mouse_move(usercmd, event.motion.xrel, event.motion.yrel);
      break;
    }
  }
}

static void text_input_event(const char *text)
{
  if (sdl.focus_gui) {
    ngui_text_input(text);
  }
}

static void key_event(usercmd_t *usercmd, int key, int action)
{
  if (key == '`' && !sdl.focus_gui) {
    sdl.focus_gui = true;
    sdl.focus_game = false;
    ngui_focus();
    SDL_StartTextInput();
  }
  
  if (key == 27) {
    sdl.focus_game = true;
    sdl.focus_gui = false;
    ngui_unfocus();
    SDL_StopTextInput();
  }
  
  if (sdl.focus_game) {
    switch (key) {
    case 'w':
      usercmd->forward = action;
      break;
    case 'a':
      usercmd->left = action;
      break;
    case 's':
      usercmd->back = action;
      break;
    case 'd':
      usercmd->right = action;
      break;
    case ' ':
      usercmd->jump = action;
      break;
    case '\x1B': //SDL_GetKeyName(): // escape key
      usercmd->esc = action;
      break;
    default:
      break;
    }
  }
  
  if (sdl.focus_gui) {
    ngui_key_event(key, action);
  }
}

static void mouse_event(usercmd_t *usercmd, int button, int action)
{
  if (button == 1)
    usercmd->attack1 = action;
}

static void mouse_move(usercmd_t *usercmd, int d_x, int d_y)
{
  usercmd->d_pitch = d_y;
  usercmd->d_yaw = d_x;
}

bool sdl_init(int width, int height, const char *title)
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  
  sdl.window = SDL_CreateWindow(
    title,
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    width,
    height,
    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  
  if (!sdl.window) {
    LOG_ERROR("failed to create SDL window");
    return false;
  }
  
  sdl.gl_context = SDL_GL_CreateContext(sdl.window);
  
  if (!sdl.gl_context) {
    LOG_ERROR("failed to create GL context");
    return false;
  }
  
  sdl.focus_gui = false;
  sdl.focus_game = true;
  
  SDL_StopTextInput();
  
  return true;
}

void sdl_swap()
{
  SDL_GL_SwapWindow(sdl.window);
}

void sdl_quit()
{
  SDL_GL_DeleteContext(sdl.gl_context);
  SDL_DestroyWindow(sdl.window);
  SDL_Quit();
}

bool sdl_should_not_quit()
{
  return !sdl.quit;
}
