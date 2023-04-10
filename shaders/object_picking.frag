#version 450

layout (location = 0) out vec4 outColor;

layout(push_constant) uniform push
{
	uint r;
	uint g;
	uint b;
} push;

void main() {
	gl_Position = 
	outColor = vec4(push.r/255.0, push.g/255.0, push.b/255.0, 1.0);
}