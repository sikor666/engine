#version 420 core

layout (binding = 0) uniform sampler2D tex;

layout (location = 0) out vec4 color;

in VS_OUT
{
    vec3 tc;
    vec4 color;
} fs_in;

void main(void)
{
    // Write final color to the framebuffer
    color = texture(tex, fs_in.color.xy + fs_in.tc.xy / textureSize(tex, 0));

    // color = fs_in.color;
    // color = texture(tex, gl_FragCoord.xy / textureSize(tex, 0));
    // color = fs_in.color;
    // color = texelFetch(tex, ivec2(gl_FragCoord.xy), 0);
    // gl_PointCoord
    // if (gl_FragCoord.xy != fs_in.tc.xy)
    // {
    //    color = texture(tex, fs_in.tc.xy);
    // }

    //if (gl_PointCoord.xy == fs_in.tc.xy)
    //{
    //    color = vec4(0.0, 0.0, 0.0, 1.0);
    //}
    //else
    //{
    //    color = texture(tex, fs_in.tc.xy);
    //}

    // color = vec4(fs_in.tc.x, fs_in.tc.y, fs_in.tc.z, 1.0);
    // color = vec4(gl_FragCoord.x, gl_FragCoord.y, gl_FragCoord.z, 1.0);

    // color = texture(tex, fs_in.tc.xy);
}
