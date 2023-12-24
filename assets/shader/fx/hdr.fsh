
void frag_pass()
{
  const float gamma = 0.45;
  const float exposure = 3.0;
  
  vec3 hdr_color = get_color(get_uv()).rgb;
  
  vec3 mapped = vec3(1.0) - exp(-hdr_color * exposure);
  mapped = pow(mapped, vec3(1.0 / gamma));
  
  set_frag(vec4(mapped, 1.0));
}
