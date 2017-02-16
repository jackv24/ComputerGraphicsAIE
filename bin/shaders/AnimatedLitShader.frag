// shadertype=glsl
#version 410
in vec4 worldNormal;
in vec4 worldPosition;
in vec2 uv;
out vec4 fragColor;

uniform vec4 cameraPosition;
uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D specular;

void main() 
{ 
	//Hardcoded light direction
	vec4 light = vec4(0.5, 0.7, 0.5, 0);

	//Diffuse
	float intensity = clamp(dot(light, worldNormal), 0, 1);

	//Specular
	vec4 toCamera = normalize(worldPosition - cameraPosition);
	vec4 refl = reflect(toCamera, worldNormal);
	float specPower = clamp(dot(refl, light), 0, 1);
	//Raised to the power of two - controls highlight tightness
	specPower = pow(specPower, 4) * 2;

	vec4 spec = texture(specular, uv) * specPower;

	//Diffuse colour
	//vec2 uv = worldPosition.xy;
	//Col is flipped texture
	vec4 col = texture(diffuse, uv);

	//Final colour
    fragColor = col * (intensity + vec4(spec.xyz, 1) + 0.2);
}