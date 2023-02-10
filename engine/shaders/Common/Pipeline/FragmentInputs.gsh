#ifndef COMMON_PIPELINE_FRAGMENTINPUTS_GSH_
#define COMMON_PIPELINE_FRAGMENTINPUTS_GSH_

struct FragmentInputs 
{
    [[vk::location(0)]] float4 posCS : SV_POSITION;
    [[vk::location(1)]] float3 posWS : POSITION1;
    [[vk::location(2)]] float2 uv : TEXCOORD;
    [[vk::location(3)]] float3 normalWS : TEXCOORD1;
    [[vk::location(4)]] float3 tangentWS : TEXCOORD2;
    [[vk::location(5)]] uint instanceID : TEXCOORD3;
    [[vk::location(6)]] uint flags : TEXCOORD4;
};

#endif // COMMON_PIPELINE_FRAGMENTINPUTS_GSH_