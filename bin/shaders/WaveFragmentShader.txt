#version 410

in vec4 vColour;
out vec4 fragColor;

uniform float time;

void main()
{
	vec4 col = vColour;

	col.r += sin(time);
	col.g += sin(time / 2);
	col.b += sin(time / 5);

	fragColor = col;
}