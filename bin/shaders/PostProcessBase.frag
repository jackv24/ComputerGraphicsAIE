#version 410
in vec2 fTexCoord;
out vec4 fragColor;

uniform sampler2D target;

void main() 
{
    fragColor = texture(target, fTexCoord);
}