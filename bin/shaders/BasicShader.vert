#version 410
layout(location=0) in vec4 position;
layout(location=1) in vec4 colour;
out vec4 vColour;

uniform mat4 projectionViewWorldMatrix;

void main() 
{ 
    vColour = colour; 
    gl_Position = MVP * position;
}