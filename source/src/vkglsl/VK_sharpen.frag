#version 450

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 textureSize;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

#define sharp_clamp ( 0.050000 )
#define sharp_strength ( 0.500000 )
#define CoefLuma vec3(0.2126, 0.7152, 0.0722)

void main()
{
	vec3 origset = texture(texSampler, fragTexCoord).rgb;
	vec3 sharp_strength_luma = (CoefLuma * sharp_strength);
	vec2 textureSize2 = vec2(textureSize.x, -textureSize.y);
	vec3 blur_sample;
	blur_sample  = texture(texSampler, fragTexCoord + textureSize2).rgb;
	blur_sample += texture(texSampler, fragTexCoord - textureSize).rgb;
	blur_sample += texture(texSampler, fragTexCoord + textureSize).rgb;
	blur_sample += texture(texSampler, fragTexCoord - textureSize2).rgb;
	blur_sample *= 0.25;
	vec3 sharp = origset - blur_sample;
	vec4 sharp_strength_luma_clamp = vec4(sharp_strength_luma * (0.5 / sharp_clamp), 0.5);
	float sharp_luma = clamp(dot(vec4(sharp, 1.0), sharp_strength_luma_clamp), 0.0, 1.0);
	sharp_luma = (sharp_clamp * 2.0) * sharp_luma - sharp_clamp;
	vec3 outputcolor = origset + sharp_luma;
	outColor = vec4(clamp(outputcolor, 0.0, 1.0), 1.0);
}