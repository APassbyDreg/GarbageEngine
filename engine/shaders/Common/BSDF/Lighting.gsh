#ifndef COMMON_BSDF_LIGHTING_GSH_
#define COMMON_BSDF_LIGHTING_GSH_

#include "Common/LightData.gsh"

float PointLightFalloff(float dist, float r)
{
    float dist2 = dist * dist;
    float linear_falloff2 = dist2 / (r * r);
    return saturate((1.0 - linear_falloff2 * linear_falloff2) / (1.0f + dist2));
}

void IncidentPointLight(uint light_id, float3 posWS, float3 normalWS, out float3 radiance, out float3 lightdirWS)
{
    float3 surface_to_light = point_lights[light_id].position - posWS;
    float dist = max(length(surface_to_light), point_lights[light_id].radius);
    lightdirWS = normalize(surface_to_light);
    radiance = point_lights[light_id].intensity * PointLightFalloff(dist, point_lights[light_id].falloff_range) * max(0.0, dot(normalWS, lightdirWS));
}

void IncidentSpotLight(uint light_id, float3 posWS, float3 normalWS, out float3 radiance, out float3 lightdirWS)
{
    // TODO: Implement this later
    radiance = 0.0;
}

void IncidentDirectionLight(uint light_id, float3 posWS, float3 normalWS, out float3 radiance, out float3 lightdirWS)
{
    lightdirWS = -direction_lights[light_id].direction;
    radiance = direction_lights[light_id].intensity * max(0.0, dot(normalWS, lightdirWS));
}

#endif // COMMON_BSDF_LIGHTING_GSH_