#version 430 core
#pragma debug(on)
#pragma optimize(off)

layout(triangles) in;

//uniform vec3 disco;

patch in vec4 patch_color;
out vec4 vs_color;

float TriArea2D(float x1, float y1, float x2, float y2, float x3, float y3)
{
    return (x1 - x2)*(y2 - y3) - (x2 - x3)*(y1 - y2);
}

// Compute barycentric coordinates (u, v, w) for
// point p with respect to triangle (a, b, c)
void Barycentric(vec3 a, vec3 b, vec3 c, vec3 p, out float u, out float v, out float w)
{
    // Unnormalized triangle normal
    vec3 m = cross(b - a, c - a);
    // Nominators and one-over-denominator for u and v ratios
    float nu, nv, ood;
    // Absolute components for determining projection plane
    float x = abs(m.x), y = abs(m.y), z = abs(m.z);
    // Compute areas in plane of largest projection
    if (x >= y && x >= z) {
        // x is largest, project to the yz plane
        nu = TriArea2D(p.y, p.z, b.y, b.z, c.y, c.z); // Area of PBC in yz plane
        nv = TriArea2D(p.y, p.z, c.y, c.z, a.y, a.z); // Area of PCA in yz plane
        ood = 1.0f / m.x; // 1/(2*area of ABC in yz plane)
    }
    else if (y >= x && y >= z) {
        // y is largest, project to the xz plane
        nu = TriArea2D(p.x, p.z, b.x, b.z, c.x, c.z);
        nv = TriArea2D(p.x, p.z, c.x, c.z, a.x, a.z);
        ood = 1.0f / -m.y;
    }
    else {
        // z is largest, project to the xy plane
        nu = TriArea2D(p.x, p.y, b.x, b.y, c.x, c.y);
        nv = TriArea2D(p.x, p.y, c.x, c.y, a.x, a.y);
        ood = 1.0f / m.z;
    }
    u = nu * ood;
    v = nv * ood;
    w = 1.0f - u - v;
}

// Test if point p is contained in triangle (a, b, c)
bool TestPointTriangle(vec3 p, vec3 a, vec3 b, vec3 c)
{
    float u, v, w;
    Barycentric(a, b, c, p, u, v, w);
    return v >= 0.0f && w >= 0.0f && (v + w) <= 1.0f;
}

void main(void)
{
    gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position) +
                  (gl_TessCoord.y * gl_in[1].gl_Position) +
                  (gl_TessCoord.z * gl_in[2].gl_Position);

    /*
    if (TestPointTriangle(disco, gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz))
    {
        vs_color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    }
    else
    {
        vs_color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    */

    vs_color = patch_color;
}
