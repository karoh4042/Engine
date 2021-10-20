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
    float3 lpos : POSITION;
};

cbuffer SHADER_VARS : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    //light
    float4 lightDir;

};

V_OUT main(InputVertex input)
{
    V_OUT output = (V_OUT) 0;
    //skybox
    //local position can be used for uvw
    // technically passing in a vector that points to the texture location
    output.lpos = input.xyz;
    output.opos = float4(input.xyz, 1);
    output.norm.rgb = input.nrm;
    output.otex = input.uvw;
    //math (shader intrinsics)
    output.opos = mul(output.opos, worldMatrix);
    output.opos = mul(output.opos, viewMatrix);
    output.opos = mul(output.opos, projectionMatrix);
    return output;

}