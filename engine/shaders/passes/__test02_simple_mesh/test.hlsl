struct A2V {
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float3 normal : NORMAL;
    [[vk::location(2)]] float3 tangent : TANGENT;
    [[vk::location(3)]] float3 texcoord : TEXCOORD0;
    [[vk::location(4)]] int flags : TEXCOORD1;
};

struct V2F {
    [[vk::location(0)]] float4 posCS : SV_POSITION;
};

[[vk::push_constant]]
struct PushConstants {
    float4x4 RenderMatrix;
    float4 CameraPosWS;
    float4 DebugColor;
} pc;

V2F vert(A2V i)
{
    V2F o;
    float4 p = mul(pc.RenderMatrix, float4(i.position, 1.0));
    o.posCS = p / p.w;
    return o;
}

float4 frag(V2F i) : SV_Target
{
    return pc.DebugColor;
}