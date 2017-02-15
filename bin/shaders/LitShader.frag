// shadertype=glsl
#version 410
in vec4 worldNormal;
in vec4 worldPosition;
in vec2 uv;
out vec4 fragColor;

uniform vec4 cameraPosition;
uniform sampler2D diffuse;

void main() 
{ 
	//Hardcoded light direction
	vec4 light = vec4(0.5, 0.7, 0.5, 0);

	//Diffuse
	float intensity = clamp(dot(light, worldNormal), 0, 1);

	//Specular
	vec4 toCamera = normalize(worldPosition - cameraPosition);
	vec4 refl = reflect(toCamera, worldNormal);
	float specular = clamp(dot(refl, light), 0, 1);
	//Raised to the power of two - controls highlight tightness
	specular = pow(specular, 16);

	//Diffuse colour
	//vec2 uv = worldPosition.xy;
	//Col is flipped texture
	vec4 col = texture(diffuse, vec2(uv.s, 1-uv.t));

	//Final colour
    fragColor = col * (intensity + vec4(specular, specular, specular, 1) + 0.2);
}