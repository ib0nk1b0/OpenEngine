#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Albedo;
layout(location = 3) in float a_Roughness;
layout(location = 4) in float a_Metalic;
layout(location = 5) in int a_EntityID;
layout(location = 6) in vec4 a_Transform0;
layout(location = 7) in vec4 a_Transform1;
layout(location = 8) in vec4 a_Transform2;
layout(location = 9) in vec4 a_Transform3;

uniform mat4 u_ViewProjection;

layout(location = 0) out vec3 v_Albedo;
layout(location = 1) out float v_Roughness;
layout(location = 2) out float v_Metalic;
layout(location = 3) out vec3 v_Normal;
layout(location = 4) out vec3 FragPos;
layout(location = 5) out flat int v_EntityID;

void main()
{
    mat4 Transform = mat4(a_Transform0, a_Transform1, a_Transform2, a_Transform3);
    v_Albedo = a_Albedo;
    v_Roughness = a_Roughness;
    v_Metalic = a_Metalic;
    FragPos = vec3(Transform * vec4(a_Position, 1.0));
    v_Normal = mat3(transpose(inverse(Transform))) * a_Normal;
    v_EntityID = a_EntityID;
    gl_Position = u_ViewProjection * Transform * vec4(a_Position, 1.0);
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

layout(location = 0) in vec3 v_Albedo;
layout(location = 1) in float v_Roughness;
layout(location = 2) in float v_Metalic;
layout(location = 3) in vec3 v_Normal;
layout(location = 4) in vec3 FragPos;
layout(location = 5) in flat int v_EntityID;

const float PI = 3.14159265359;

float DistributionGGX(float NdotH, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float denom = NdotH * NdotH * (a2 - 1.0) + 1.0;
    denom = PI * denom * denom;
    return a2 / max(denom, 0.0000001);
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    float ggx1 = NdotV / (NdotV * (1.0 - k) + k);
    float ggx2 = NdotL / (NdotL * (1.0 - k) + k);
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float HdotV, vec3 baseReflectivity)
{
    return baseReflectivity + (1.0 - baseReflectivity) * pow(1.0 - HdotV, 5.0);
}

void main()
{
    vec3 N = normalize(v_Normal);
    vec3 V = normalize(u_CameraPosition - FragPos);

    vec3 baseReflectivity = mix(vec3(0.04), v_Albedo, v_Metalic);

    vec3 Lo = vec3(0.0);

    //vec3 L = normalize(u_LightPosition - FragPos);
    vec3 L = normalize(u_LightPosition);
    vec3 H = normalize(V + L);
    float distance = length(u_LightPosition - FragPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radience = u_LightColor;

    float NdotV = max(dot(N, V), 0.0000001);
    float NdotL = max(dot(N, L), 0.0000001);
    float HdotV = max(dot(H, V), 0.0);
    float NdotH = max(dot(N, H), 0.0);

    float D = DistributionGGX(NdotH, v_Roughness);
    float G = GeometrySmith(NdotV, NdotL, v_Roughness);
    vec3 F = FresnelSchlick(HdotV, baseReflectivity);

    vec3 specular = D * G * F;
    specular /= 4.0 * NdotV * NdotL;

    vec3 kD = vec3(1.0) - F;
    kD *= 1.0 - v_Metalic;
    Lo += (kD * v_Albedo / PI + specular) * radience * NdotL;

    vec3 ambient = vec3(u_AmbientIntensity) * v_Albedo * u_LightColor;

    vec3 result =  ambient + Lo;
    result = result / (result + vec3(1.0));
    result = pow(result, vec3(1.0/2.2));

    color = vec4(result, 1.0);
    color2 = v_EntityID;
}