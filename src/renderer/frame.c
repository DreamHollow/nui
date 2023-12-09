#include "frame.h"

#include "mesh.h"
#include "shader.h"
#include "camera.h"

typedef struct {
  mesh_t quad;
  GLuint shader;
} frame_def_t;

static frame_def_t frame_def;

bool frame_init()
{
  vertex_t quad_vertices[] = {
    { .pos = { -1.0f, -1.0f, 0.0f }, .uv = { 0.0f, 0.0f } },
    { .pos = { -1.0f, +1.0f, 0.0f }, .uv = { 0.0f, 1.0f } },
    { .pos = { +1.0f, -1.0f, 0.0f }, .uv = { 1.0f, 0.0f } },
    { .pos = { -1.0f, +1.0f, 0.0f }, .uv = { 0.0f, 1.0f } },
    { .pos = { +1.0f, +1.0f, 0.0f }, .uv = { 1.0f, 1.0f } },
    { .pos = { +1.0f, -1.0f, 0.0f }, .uv = { 1.0f, 0.0f } }
  };
  
  if (!mesh_buffer_new(&frame_def.quad, quad_vertices, 6)) {
    return false;
  }
  
  return true;
}

bool frame_new(frame_t *frame, const char *fx, int width, int height)
{
  glGenTextures(1, &frame->buffer);
  glBindTexture(GL_TEXTURE_2D, frame->buffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  glGenFramebuffers(1, &frame->fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, frame->fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame->buffer, 0);
  
  glGenRenderbuffers(1, &frame->rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, frame->rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, frame->rbo);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  if (!fx_shader_load(&frame->shader, fx, "")) {
    return false;
  }
  
  frame->width = width;
  frame->height = height;
  
  return true;
}

void frame_begin(frame_t *frame)
{
  glBindFramebuffer(GL_FRAMEBUFFER, frame->fbo);
  camera_set_viewport(0, 0, frame->width, frame->height);
}

void frame_end(frame_t *frame)
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void frame_draw(frame_t *frame, int x, int y, int width, int height)
{
  glUseProgram(frame->shader);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, frame->buffer);
  
  glViewport(x, y, width, height);
  glScissor(x, y, width, height);
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  mesh_draw(frame_def.quad);
}
