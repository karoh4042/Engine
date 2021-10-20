Texture2D myTexture : register(t0);
Texture2D secondTexture : register(t1);
sampler mySampler : register(s0);

struct V_OUT
{
    float4 opos : SV_Position;
    float3 norm : NORMAL;
    float3 otex : TEXTCOORD;
    float3 wpos : WORLD;
};

float4 main() : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 0.0f);
}