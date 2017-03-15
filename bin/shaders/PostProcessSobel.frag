#version 410
in vec2 fTexCoord;
out vec4 fragColor;

uniform sampler2D target;

vec4 Sobel()
{
	vec2 texel = 1.0f / textureSize(target, 0).xy;

	//X
	vec4 colX = texture(target, fTexCoord) * 0;
	colX += texture(target, fTexCoord + vec2(texel.x, 0)) * -2;
	colX += texture(target, fTexCoord + vec2(-texel.x, 0)) * 2;
	colX += texture(target, fTexCoord + vec2(-texel.x, texel.y)) * -1;
	colX += texture(target, fTexCoord + vec2(-texel.x, -texel.y)) * -1;
	colX += texture(target, fTexCoord + vec2(texel.x, texel.y)) * 1;
	colX += texture(target, fTexCoord + vec2(texel.x, -texel.y)) * 1;
	colX.a = 1.0f;

	//Y
	vec4 colY = texture(target, fTexCoord) * 0;
	colY += texture(target, fTexCoord + vec2(-texel.x, texel.y)) * -1;
	colY += texture(target, fTexCoord + vec2(0, texel.y)) * -2;
	colY += texture(target, fTexCoord + vec2(texel.x, texel.y)) * -1;
	colY += texture(target, fTexCoord + vec2(-texel.x, -texel.y)) * 1;
	colY += texture(target, fTexCoord + vec2(0, -texel.y)) * 2;
	colY += texture(target, fTexCoord + vec2(texel.x, -texel.y)) * 1;
	colY.a = 1.0f;

	vec4 square = vec4(2, 2, 2, 2);

	return sqrt(pow(colX, square) + pow(colY, square));
}

void main() 
{
	vec4 sobel = Sobel();
	float intensity = (sobel.x + sobel.y + sobel.z) / 3;
	vec4 edge = vec4(1-intensity, 1-intensity, 1-intensity, 1);

    fragColor = edge * texture(target, fTexCoord);
}