//texture cube not 2d
TextureCube myTexture : register(t0);
sampler mySampler : register(s0);

struct V_OUT
{
    float4 opos : SV_Position;
    float3 norm : NORMAL;
    float3 otex : TEXTCOORD;
    float3 lpos : POSITION;
};

float4 main(V_OUT input) : SV_TARGET
{
    float4 textureColor = myTexture.Sample(mySampler, input.lpos);
    return textureColor;
}
