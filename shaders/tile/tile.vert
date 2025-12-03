#version 450

layout(set = 1, binding = 0) uniform TileInfo {vec2 pos;} tinfo;

layout(location = 0) in vec2 size;
layout(location = 1) in vec2 uv;

layout(location = 0) out vec2 frag_uv;

void main(){
     gl_Position = vec4(size+tinfo.pos, 0.0, 1.0);
     frag_uv = uv;
}