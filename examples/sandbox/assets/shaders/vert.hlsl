struct Material
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 reflect;
};

cbuffer per_object : register(b0)
{
    float4x4 model;
    float4x4 view;
    float4x4 proj;
    Material mat;
};

struct VertexIn {
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 tex_coord : TEXCOORD;
};

struct VertexOut {
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float2 tex_coord : TEXCOORD;
};

VertexOut main(VertexIn vin) {
    VertexOut vout;
    vout.position = mul(proj, mul(view, mul(model, float4(vin.position, 1.0))));
    vout.normal = float4(vin.normal, 0.0f);
    vout.tex_coord = vin.tex_coord;
    return vout;
};