#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec4 colour;
out vec4 vColour;

uniform mat4 projectionViewWorldMatrix;
uniform float time;
uniform float heightScale;

void main()
{
	vColour = colour;
	vec4 p = position;

	p.y += sin(time + (position.x + position.z) / 2) * heightScale;

	gl_Position = projectionViewWorldMatrix * p;
}