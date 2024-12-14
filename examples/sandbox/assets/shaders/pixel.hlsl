struct VertexOut
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
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

void ComputeDirectionalLight(Material mat, DirectionalLight l, float3 normal, float3 to_eye, out float4 ambient, out float4 diffuse, out float4 specular)
{
    // Initialize outputs
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float3 light_vec = -l.direction;
    
    ambient = mat.ambient * l.ambient;
    
    float diffuse_factor = dot(light_vec, normal);
    
    [flatten]
    if (diffuse_factor > 0.0f)
    {
        float3 v = reflect(-light_vec, normal);
        float spec_factor = pow(max(dot(v, to_eye), 0.0f), mat.specular.w);

        diffuse = diffuse_factor * mat.diffuse * l.diffuse;
        specular = spec_factor * mat.specular * l.specular;
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
    
    ComputeDirectionalLight(mat, dir_light, pin.normal.xyz, to_eye, A, D, S);
    
    ambient += A;
    diffuse += D;
    specular += S;
    
    float4 lit_color = ambient + diffuse + specular;
    lit_color.a = mat.diffuse.a;
    
    return lit_color;
}