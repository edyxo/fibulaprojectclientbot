#version 450

layout(push_constant) uniform UniformBufferObject {
	vec2 vkSize;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexcoord;
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexcoord;

void main()
{
	gl_Position = vec4(inPosition.xy * ubo.vkSize.xy - 1.0, 0.0, 1.0);
	fragColor = inColor;
	fragTexcoord = inTexcoord;
}