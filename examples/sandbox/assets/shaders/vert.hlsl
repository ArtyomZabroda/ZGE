struct VertexIn {
  float3 position : POSITION;
};

struct VertexOut {
  float4 position : SV_POSITION;
};

VertexOut main(VertexIn vin) {
  VertexOut vout;
  vout.position = float4(vin.position, 1.0);
  return vout;
};