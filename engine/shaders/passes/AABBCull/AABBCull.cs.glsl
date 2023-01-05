struct AABB
{
    vec3 min_loc;
    vec3 max_loc;
};

layout(local_size_x = 256) in;

layout(push_constant) uniform Constants
{
    uint num_aabbs;
    mat4 mvp;
}
config;

layout(binding = 1) readonly buffer AABBBuffer { AABB aabbs[]; }
aabb_buffer;
layout(binding = 2) buffer ResultBuffer { lowp uint result[]; }
result_buffer;

const vec3 directions[8] = {vec3(0.0, 0.0, 0.0),
                            vec3(0.0, 0.0, 1.0),
                            vec3(0.0, 1.0, 0.0),
                            vec3(0.0, 1.0, 1.0),
                            vec3(1.0, 0.0, 0.0),
                            vec3(1.0, 0.0, 1.0),
                            vec3(1.0, 1.0, 0.0),
                            vec3(1.0, 1.0, 1.0)};

void main()
{
    uint      gID    = gl_GlobalInvocationID.x;
    lowp uint result = 0;
    for (int dx = 0; dx < 8; dx++)
    {
        uint idx = gID * 8 + dx;
        if (idx < config.num_aabbs)
        {
            bool inside     = false;
            AABB aabb       = aabb_buffer.aabbs[idx];
            vec3 min_to_max = aabb.max_loc - aabb.min_loc;
            for (int i = 0; i < 8; i++)
            {
                vec3 point      = aabb.min_loc + min_to_max * directions[i];
                vec4 clip_space = config.mvp * vec4(point, 1.0);
                if (clip_space.x >= -clip_space.w && clip_space.x <= clip_space.w && clip_space.y >= -clip_space.w &&
                    clip_space.y <= clip_space.w && clip_space.z >= -clip_space.w && clip_space.z <= clip_space.w)
                {
                    inside = true;
                    break;
                }
            }
            if (inside)
            {
                result |= 1 << dx;
            }
        }
    }
    result_buffer.result[gID] = result;
}