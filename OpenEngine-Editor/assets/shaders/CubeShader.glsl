#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Albedo;
layout(location = 3) in float a_Roughness;
layout(location = 4) in float a_Metalic;
layout(location = 5) in int a_EntityID;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

layout(location = 0) out vec4 v_Color;
layout(location = 1) out vec3 v_Normal;
layout(location = 3) out vec3 FragPos;
layout(location = 4) out flat int v_EntityID;

void main()
{
    v_Color = vec4(a_Albedo, 1.0);
    FragPos = vec3(u_Model * vec4(a_Position, 1.0));
    v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
    //v_Normal = a_Normal;
    v_EntityID = a_EntityID;
    vec4 Position = u_Model * vec4(a_Position, 1.0);
    gl_Position = u_ViewProjection * Position;
    //gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

uniform vec3 u_LightPosition;
uniform vec3 u_LightColor;
uniform float u_AmbientIntensity;
uniform float u_SpecularStrength;
uniform vec3 u_CameraPosition;

layout(location = 0) in vec4 v_Color;
layout(location = 1) in vec3 v_Normal;
layout(location = 3) in vec3 FragPos;
layout(location = 4) in flat int v_EntityID;

void main()
{
    //ambient
    vec3 ambient = u_AmbientIntensity * u_LightColor;

    //diffuse
    vec3 normal = normalize(v_Normal);
    //vec3 lightDirection = normalize(u_LightPosition - FragPos);
    vec3 lightDirection = normalize(u_LightPosition);
    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = diff * u_LightColor;
    
    //specular
    vec3 viewDirection = normalize(u_CameraPosition - FragPos);
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
    vec3 specular = u_SpecularStrength * spec * u_LightColor;

    //attenuation
    //float distance = length(u_LightPosition - FragPos);
    //float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));

    //ambient *= attenuation;
    //diffuse *= attenuation;
    //specular *= attenuation;

    //vec4 result = vec4((diffuse), 1.0) * v_Color;
    //vec4 result = vec4((specular), 1.0) * v_Color;

    //vec4 result = vec4((ambient), 1.0) * v_Color;
    //vec4 result = vec4((u_AmbientIntensity + diffuse), 1.0) * v_Color;
    vec4 result = vec4((ambient + diffuse + specular), 1.0) * v_Color;

    //vec4 result = vec4(diff, 0.0, 0.0, 1.0);

    color = result;
    
    //color = v_Color;
    color2 = v_EntityID;
}