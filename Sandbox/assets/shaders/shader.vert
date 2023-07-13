#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
// layout(location = 2) in vec2 a_TexCoord;
// layout(location = 3) in float a_TexIndex;
// layout(location = 4) in float a_Scale;
// layout(location = 5) in int a_EntityID;

layout(location = 0) out vec4 v_Color;

// layout(push_constant) uniform Push {
// 	vec2 Offset;
// 	vec4 Color;
// } push;

void main() {
	gl_Position = vec4(a_Position, 1.0);
	v_Color = a_Color;
}