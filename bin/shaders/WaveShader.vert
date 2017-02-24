#version 410
layout(location=0) in vec4 position;
layout(location=1) in vec2 uvs;
layout(location=2) in vec4 normal;
layout(location=3) in vec4 tangent;

out vec4 worldPosition;
out vec2 uv;
out vec4 worldNormal;
out vec3 vNormal;
out vec3 vTangent;
out vec3 vBiTangent;

uniform mat4 MVP;
uniform mat4 M;
uniform float time;

void main() 
{ 
	vec4 p = position;
	p.z += sin(time + (position.x + position.y) / 2);

    worldNormal = normalize(M * normal); 
	worldPosition = M * p;

	vNormal = normal.xyz;
	vTangent = tangent.xyz;
	vBiTangent = cross(vNormal, vTangent);

	gl_Position = MVP * p;

	uv = uvs;
}