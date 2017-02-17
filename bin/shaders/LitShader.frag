// shadertype=glsl
#version 410
in vec4 worldPosition;
in vec2 uv;
in vec4 worldNormal;
in vec3 vNormal;
in vec3 vTangent;
in vec3 vBiTangent;

out vec4 fragColor;

uniform vec4 cameraPosition;
uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D specular;
uniform vec4 light;

void main() 
{ 
	//Normal map
	mat3 TBN = mat3(normalize(vTangent), normalize(vBiTangent), normalize(vNormal));
	vec3 N = texture(normal, uv).xyz * 2 - 1;

	vec4 norm = vec4(TBN * N, 0);

	//Diffuse
	float intensity = clamp(dot(light, norm), 0, 1);

	//Specular
	vec4 toCamera = normalize(worldPosition - cameraPosition);
	vec4 refl = reflect(toCamera, norm);
	float specPower = clamp(dot(refl, light), 0, 1);
	//Raised to the power of two - controls highlight tightness
	specPower = pow(specPower, 4) * 5;

	vec4 spec = texture(specular, uv) * specPower;

	//Diffuse colour
	vec4 col = texture(diffuse, uv);

	//Final colour
    fragColor = col * (intensity + vec4(spec.xyz, 1) + 0.2);

}