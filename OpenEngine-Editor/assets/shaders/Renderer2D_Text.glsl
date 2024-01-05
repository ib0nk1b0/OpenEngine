#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_DistanceFactor;
layout(location = 4) in int a_EntityID;

uniform mat4 u_ViewProjection;

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
};

layout(location = 0) out VertexOutput Output;
layout(location = 2) out flat int v_EntityID;
layout(location = 3) out float v_DistanceFactor;

void main()
{
	Output.Color = a_Color;
	Output.TexCoord = a_TexCoord;
	v_EntityID = a_EntityID;
	v_DistanceFactor = a_DistanceFactor;

	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
};

layout(location = 0) in VertexOutput Input;
layout(location = 2) in flat int v_EntityID;
layout(location = 3) in float v_DistanceFactor;

uniform sampler2D u_FontAtlas;

float screenPxRange() {
	const float pxRange = 2.0; // set to distance field's pixel range
	vec2 unitRange = vec2(pxRange)/vec2(textureSize(u_FontAtlas, 0));
	vec2 screenTexSize = vec2(1.0)/fwidth(Input.TexCoord);
	return max(0.5*dot(unitRange, screenTexSize), 1.0);
}

float median(float r, float g, float b) {
	return max(min(r, g), min(max(r, g), b));
}

void main()
{
	vec4 color = texture(u_FontAtlas, Input.TexCoord);
	vec3 s = texture(u_FontAtlas, Input.TexCoord).rgb;
    float sigDist = v_DistanceFactor*(median(s.r, s.g, s.b) - 0.5);
    float opacity = clamp(sigDist + 0.5, 0.0, 1.0);


	if (opacity == 0.0)
		discard;

	vec4 bgColor = vec4(0.0);
	o_Color = mix(bgColor, Input.Color, opacity);
	//o_Color = color;
	if (o_Color.a == 0.0)
		discard;
	
	o_EntityID = v_EntityID;
}