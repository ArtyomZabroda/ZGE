cbuffer per_object
{
  float4x4 model;
   float4x4 view;
   float4x4 proj;
};

struct VertexIn {
  float3 position : POSITION;
};

struct VertexOut {
  float4 position : SV_POSITION;
};

VertexOut main(VertexIn vin) {
  VertexOut vout;
  vout.position = mul(proj, mul(view, mul(model, float4(vin.position, 1.0))));
  return vout;
};