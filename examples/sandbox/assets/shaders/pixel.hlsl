struct VertexOut
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float2 tex_coord : TEXCOORD;
};

struct Material
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 reflect;
};

struct DirectionalLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float3 direction;
    float pad;
};

struct PointLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    
    float3 position;
    float range;
    
    float3 attenuation;
    float pad;
};

struct SpotLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    
    float3 position;
    float range;
    
    float3 direction;
    float spot;
    
    float3 attenuation;
    float pad;
};

cbuffer per_object : register(b0)
{
    float4x4 model;
    float4x4 view;
    float4x4 proj;
    Material mat;
};

cbuffer per_frame : register(b1)
{
    DirectionalLight dir_light;
    float3 eye_pos;
    float pad;
};

Texture2D diffuse_map;
Texture2D specular_map;

SamplerState sam1
{
    Filter = MIN_MAG_MIP_LINEAR;
};

void ComputeDirectionalLight(Material mat, DirectionalLight l, float3 normal, float3 to_eye, float2 tex_coords, out float4 ambient, out float4 diffuse, out float4 specular)
{
    // Initialize outputs
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float3 light_vec = -l.direction;
    
    ambient = diffuse_map.Sample(sam1, tex_coords) * l.ambient;
    
    float diffuse_factor = dot(light_vec, normal);
    
    [flatten]
    if (diffuse_factor > 0.0f)
    {
        float3 v = reflect(-light_vec, normal);
        float spec_factor = pow(max(dot(v, to_eye), 0.0f), mat.specular.w);

        
        diffuse = diffuse_factor * l.diffuse * diffuse_map.Sample(sam1, tex_coords);
        specular = spec_factor * l.specular * specular_map.Sample(sam1, tex_coords);
    }
    
}

float4 main(VertexOut pin) : SV_Target {
    // Interpolating normal can unnormalize it, so normalize it.
    pin.normal = normalize(pin.normal);
    
    float3 to_eye = normalize(eye_pos - pin.position.xyz);
    
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float4 A, D, S;
    
    ComputeDirectionalLight(mat, dir_light, pin.normal.xyz, to_eye, pin.tex_coord, A, D, S);
    
    ambient += A;
    diffuse += D;
    specular += S;
    
    float4 lit_color = ambient + diffuse + specular;
    lit_color.a = mat.diffuse.a;
    
    return lit_color;
}