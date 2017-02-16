#version 410
layout(location=0) in vec4 position;
layout(location=1) in vec4 normal;
layout(location=2) in vec2 uvs;
layout(location=3) in vec4 weights;
layout(location=4) in vec4 indices;

out vec4 worldPosition;
out vec4 worldNormal;
out vec2 uv;

uniform mat4 MVP;
uniform mat4 M;

const int MAX_BONES = 128;
uniform mat4 bones[MAX_BONES];

void main() 
{ 
	//Cast indices to integers so they can index an array
	ivec4 index = ivec4(indices);

	//Sample bones and blend up to 4
	vec4 P = bones[index.x] * position * weights.x;
	P += bones[index.y] * position * weights.y;
	P += bones[index.z] * position * weights.z;
	P += bones[index.w] * position * weights.w;

	//Transform normals for animation
	vec4 norm = vec4(normal.xyz, 0);
	vec4 N = bones[index.x] * norm * weights.x;
	N += bones[index.y] * norm * weights.y;
	N += bones[index.z] * norm * weights.z;
	N += bones[index.w] * norm * weights.w;

    worldNormal = normalize(M * N); 
	worldPosition = M * P;

	gl_Position = MVP * P;

	uv = uvs;
}