struct AssembledVertex
{
    uint id : SV_VertexID;
};

struct VertexStageOutput
{
    float4 sv_position : SV_Position;
};

static float2 positions[3] = {
    float2(0.0, -0.5),
    float2(0.5, 0.5),
    float2(-0.5, 0.5)
};

[shader("vertex")]
VertexStageOutput vertexMain(AssembledVertex vertex)
{
    VertexStageOutput output;
    output.sv_position = float4(positions[vertex.id], 0.0, 1.0);
    return output;
}

[shader("fragment")]
float4 fragmentMain()
{
    return float4(1, 0, 0, 1);
}