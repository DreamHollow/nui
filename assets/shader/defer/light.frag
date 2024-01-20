out vec4 frag_color;

uniform samplerCube u_skybox;

void main()
{
  if (get_frag_pos().z < 0.0) {
    frag_color = texture(u_skybox, vs_ray);
    return;
  }
  
  vec3 frag_normal = get_frag_normal();
  vec3 frag_pos = get_world_pos();
  
  vec3 light = vec3(0.0);
  vec3 fog = vec3(0.0);
  
  vec3 I = normalize(frag_pos - get_view_pos());
  vec3 R = reflect(I, normalize(frag_normal));
  
  for (int i = 0; i < MAX_POINTS; i++) {
    if (points[i].intensity <= 0.0) {
      continue;
    }
    
    float shadow = 1.0 - calc_point_shadow(i, frag_pos);
    
    light += calc_light(frag_pos, frag_normal, 0.3, i) * shadow;
    fog += calc_fog(frag_pos, frag_normal, i);
  }
  
  float ao = calc_ssao(u_pos, frag_pos, frag_normal);
  
  light += 0.01;
  light *= ao * ao;
  light += 0.01 * texture(u_skybox, R).rgb;
  
  frag_color = get_diffuse() * vec4(light, 1.0) + vec4(fog, 1.0);
}
