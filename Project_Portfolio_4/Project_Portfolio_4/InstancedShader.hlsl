#pragma pack_matrix(row_major)

struct InputVertex
{
    float3 xyz : POSITION;
    float3 uvw : TEXTCOORD;
    float3 nrm : NORMAL;
    
};
struct V_OUT
{
    float4 opos : SV_Position;
    float3 norm : NORMAL;
    float3 otex : TEXTCOORD;
    float3 wpos : WORLD;
};

cbuffer SHADER_VARS : register(b0)
{
    float4x4 worldMatrix[5];
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

V_OUT main(InputVertex input, uint id : SV_InstanceID)
{
    V_OUT output = (V_OUT) 0;
    output.opos = float4(input.xyz, 1);
    output.norm.rgb = input.nrm;
    output.otex = input.uvw;
    
    //math (shader intrinsics)
    //id is automatically generated when calling drawinstanced
    output.opos = mul(output.opos, worldMatrix[id]);
    output.wpos = output.opos;
    output.norm = mul(float4(input.nrm, 0), worldMatrix[id]);
    output.opos = mul(output.opos, viewMatrix);
    output.opos = mul(output.opos, projectionMatrix);
    return output;

}