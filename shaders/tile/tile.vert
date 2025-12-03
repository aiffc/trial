#version 450

layout(set = 1, binding = 0) uniform TileInfo {
  vec2 pos;
  vec2 size;
} tinfo;

layout(set = 1, binding = 1) uniform RenderInfo{
  vec2 window_size;
} rinfo;

layout(location = 0) in vec2 vertex_pos;
layout(location = 1) in vec2 texture_coord;

layout(location = 0) out vec2 frag_uv;

void main(){
  // 缩放
  vec2 scale = tinfo.size / rinfo.window_size;
  vec2 poss = vertex_pos * scale;
  // 位移
  vec2 posf = tinfo.pos / rinfo.window_size * 2.0;
  posf += vec2(-1, -1); // 左下角做0,0

  gl_Position = vec4(poss + posf, 0.0, 1.0);
  frag_uv = texture_coord;
}
