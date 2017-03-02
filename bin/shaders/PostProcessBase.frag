#version 410
in vec2 fTexCoord;
out vec4 fragColor;

uniform sampler2D target;

vec4 BoxBlur()
{
	vec2 texel = 1.0f / textureSize(target, 0).xy;

	// 9-tap box kernel
	vec4 colour = texture(target, fTexCoord);
	colour += texture(target, fTexCoord + vec2(-texel.x, texel.y));
	colour += texture(target, fTexCoord + vec2(-texel.x, 0));
	colour += texture(target, fTexCoord + vec2(-texel.x, -texel.y));
	colour += texture(target, fTexCoord + vec2(0, texel.y));
	colour += texture(target, fTexCoord + vec2(0, -texel.y));
	colour += texture(target, fTexCoord + vec2(texel.x, texel.y));
	colour += texture(target, fTexCoord + vec2(texel.x, 0));
	colour += texture(target, fTexCoord + vec2(texel.x, -texel.y));

	return colour / 9;
}

vec4 Distort()
{
	vec2 mid = vec2(0.5f);

	float distanceFromCentre = distance(fTexCoord, mid);
	vec2 normalizedCoord = normalize(fTexCoord - mid);
	float bias = distanceFromCentre + sin(distanceFromCentre * 15) * 0.02f;

	vec2 newCoord = mid + bias * normalizedCoord;

	return texture(target, newCoord);
}

vec4 SqrVec4(vec4 vec)
{
	return vec4(vec.x * vec.x, vec.y * vec.y, vec.z * vec.z, 1);
}

vec4 SqrtVec4(vec4 vec)
{
	return vec4(sqrt(vec.x), sqrt(vec.y), sqrt(vec.z), 1);
}

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

	return SqrtVec4(SqrVec4(colX) + SqrVec4(colY));
}

void main() 
{
    fragColor = Sobel();
}