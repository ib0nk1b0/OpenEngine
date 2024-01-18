#type vertex
#version 330 core

layout (location = 0) in vec3 a_Position;

out vec3 v_TexCoord;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

void main()
{
    v_TexCoord = vec3(u_Model * vec4(a_Position, 1.0));
    vec4 pos = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
    gl_Position = pos.xyww;
}

#type fragment
#version 330 core

out vec4 FragColor;

in vec3 v_TexCoord

uniform samplerCube u_Skybox;

void main()
{
    FragColor = texture(u_Skybox, v_TexCoord);
}