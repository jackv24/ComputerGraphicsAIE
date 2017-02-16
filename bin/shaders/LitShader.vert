#version 410
layout(location=0) in vec4 position;
layout(location=1) in vec4 normal;
layout(location=2) in vec2 uvs;
out vec4 worldPosition;
out vec4 worldNormal;
out vec2 uv;

uniform mat4 MVP;
uniform mat4 M;

void main() 
{ 
    worldNormal = normalize(M * vec4(normal.xyz, 0)); 
	worldPosition = M * position;

	gl_Position = MVP * position;

	uv = uvs;
}