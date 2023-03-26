#version 300 es

layout (location = 0) in vec3 v_pos;

layout (std140) uniform ubo_matrices {
  mat4  mvp;
  mat4  model;
  vec3  view_pos;
  float pad[1];
};

out vec3 vs_pos;

void main()
{
  vs_pos = v_pos;
  gl_Position = mvp * vec4(v_pos, 1.0);
}
