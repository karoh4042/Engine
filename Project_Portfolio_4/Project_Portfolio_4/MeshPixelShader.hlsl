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

float4 main(V_OUT input) : SV_TARGET
{
     //texturing
    float4 textureColor = myTexture.Sample(mySampler, input.otex.xy);
    //multitexturing
    textureColor *= secondTexture.Sample(mySampler, input.otex.xy);
    float4 directionalLight;
    //directional lighting
    float4 directional_light_color = float4(1, 1, 1, 1);
    float3 directional_lightDir = { 1, -1, 0 };
    directional_lightDir = normalize(directional_lightDir);
    float4 directional_lightRatio = saturate(dot(normalize(input.norm), -directional_lightDir));
    
    directionalLight = textureColor * directional_lightRatio * directional_light_color;
    
    //point lighting
    float4 pointLight;
    float4 point_light_color = float4(0.01f, 1, 1, 1);
    float3 point_light_pos = { 1, 1, 1 };
    //looking for world space pos
    point_light_pos = point_light_pos - input.wpos.xyz;
   float dist = length(point_light_pos);
  point_light_pos /= dist;//normalizes point_light_pos
    float point_light_ratio = saturate(dot(normalize(input.norm), point_light_pos));
    //aattenuation
    //float att, constantAttenuation = 1.0, linearAttenuation = 0.22, quadraticAttenuation = 0.20;
    //att = constantAttenuation / ((1 + linearAttenuation * dist) * (1 + quadraticAttenuation * dist * dist));
    //att *= att;
    float att = 1 - saturate(dist / 8.0f);
    att *= att;
     pointLight = textureColor * point_light_color * point_light_ratio * att;
    //point_light_pos /= 0.0f + (0.2f * point_light_pos) + (0.0f * (point_light_pos * point_light_pos) + 0.0f);
    // pointLight = textureColor *  att * point_light_color * point_light_ratio;
    
    //spotlight Formula
    float4 spotlightColor = float4(1, 0, 0, 1);
    float3 spotlightPos = { 0, 5, 0 };
    float3 surfacePos =  input.wpos.xyz;
    float3 coneDir = { 0.5, -1, 0 };
    float3 spotlightDir = normalize(spotlightPos - surfacePos);
    float surfaceRatio = saturate(dot(-spotlightDir, normalize(coneDir)));
    float outerConeRatio = 0.65;
    float innerConeRatio = 0.85;
    //attenuation
    float radialAtt = 1 - saturate(length(spotlightPos - surfacePos) / 8.0f);
    float linearAtt = 1 - saturate((innerConeRatio - surfaceRatio) / (innerConeRatio - outerConeRatio));
    float spotlightAtt = radialAtt * linearAtt;
    
    float spotFactor = (surfaceRatio > outerConeRatio) ? 1 : 0;
    float3 surfaceNormal = normalize(input.norm);
    float spotlightRatio = saturate(dot(spotlightDir, surfaceNormal));
    float4 spotLight = spotFactor * spotlightRatio * spotlightColor * textureColor * spotlightAtt; 
    
    return saturate(pointLight + directionalLight + spotLight);
}