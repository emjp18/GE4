#define PI 3.14159265359
#define LIGHT_FAR_PLANE 800
#define CAMERA_NEAR_Z 0.01f
#define CAMERA_FAR_Z 800.0f 
#define CAMERA_SHADOW_NEAR_Z 1
#define CAMERA_SHADOW_FAR_Z 25

static const float3 gAccelW = { 0.0f, 7.8f, 0.0f };
static const float3 gAccelWR = { -1.0f, -9.8f, 0.0f };
static const float g_lineThickness = 0.09f;
static const float G_SCATTERING = 0.25;
float4 permute(float4 x)
{
    return fmod(((x * 34.0) + 1.0) * x, 289.0);
}

float4 taylorInvSqrt(float4 r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}

float3 fade(float3 t)
{
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float cnoise(float3 P)
{
    float3 Pi0 = floor(P); // Integer part for indexing
    float3 Pi1 = Pi0 + float3(1.0, 1.0, 1.0); // Integer part + 1
    Pi0 = fmod(Pi0, 289.0);
    Pi1 = fmod(Pi1, 289.0);
    float3 Pf0 = frac(P); // Fractional part for interpolation
    float3 Pf1 = Pf0 - float3(1.0, 1.0, 1.0); // Fractional part - 1.0
    float4 ix = float4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
    float4 iy = float4(Pi0.yy, Pi1.yy);
    float4 iz0 = Pi0.zzzz;
    float4 iz1 = Pi1.zzzz;

    float4 ixy = permute(permute(ix) + iy);
    float4 ixy0 = permute(ixy + iz0);
    float4 ixy1 = permute(ixy + iz1);

    float4 gx0 = ixy0 / 7.0;
    float4 gy0 = frac(floor(gx0) / 7.0) - 0.5;
    gx0 = frac(gx0);
    float4 gz0 = float4(0.5, 0.5, 0.5, 0.5) - abs(gx0) - abs(gy0);
    float4 sz0 = step(gz0, float4(0.0, 0.0, 0.0, 0.0));
    gx0 -= sz0 * (step(0.0, gx0) - 0.5);
    gy0 -= sz0 * (step(0.0, gy0) - 0.5);

    float4 gx1 = ixy1 / 7.0;
    float4 gy1 = frac(floor(gx1) / 7.0) - 0.5;
    gx1 = frac(gx1);
    float4 gz1 = float4(0.5, 0.5, 0.5, 0.5) - abs(gx1) - abs(gy1);
    float4 sz1 = step(gz1, float4(0.0, 0.0, 0.0, 0.0));
    gx1 -= sz1 * (step(0.0, gx1) - 0.5);
    gy1 -= sz1 * (step(0.0, gy1) - 0.5);

    float3 g000 = float3(gx0.x, gy0.x, gz0.x);
    float3 g100 = float3(gx0.y, gy0.y, gz0.y);
    float3 g010 = float3(gx0.z, gy0.z, gz0.z);
    float3 g110 = float3(gx0.w, gy0.w, gz0.w);
    float3 g001 = float3(gx1.x, gy1.x, gz1.x);
    float3 g101 = float3(gx1.y, gy1.y, gz1.y);
    float3 g011 = float3(gx1.z, gy1.z, gz1.z);
    float3 g111 = float3(gx1.w, gy1.w, gz1.w);

    float4 norm0 = taylorInvSqrt(float4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
    g000 *= norm0.x;
    g010 *= norm0.y;
    g100 *= norm0.z;
    g110 *= norm0.w;
    float4 norm1 = taylorInvSqrt(float4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
    g001 *= norm1.x;
    g011 *= norm1.y;
    g101 *= norm1.z;
    g111 *= norm1.w;

    float n000 = dot(g000, Pf0);
    float n100 = dot(g100, float3(Pf1.x, Pf0.yz));
    float n010 = dot(g010, float3(Pf0.x, Pf1.y, Pf0.z));
    float n110 = dot(g110, float3(Pf1.xy, Pf0.z));
    float n001 = dot(g001, float3(Pf0.xy, Pf1.z));
    float n101 = dot(g101, float3(Pf1.x, Pf0.y, Pf1.z));
    float n011 = dot(g011, float3(Pf0.x, Pf1.yz));
    float n111 = dot(g111, Pf1);

    float3 fade_xyz = fade(Pf0);
    float4 n_z = lerp(float4(n000, n100, n010, n110), float4(n001, n101, n011, n111), fade_xyz.z);
    float2 n_yz = lerp(n_z.xy, n_z.zw, fade_xyz.y);
    float n_xyz = lerp(n_yz.x, n_yz.y, fade_xyz.x);
    return 2.2 * n_xyz;
}




float rgb2luma(float3 rgb) {
    return sqrt(dot(rgb, float3(0.299, 0.587, 0.114)));
}

float3 CalculateNormalMapping(float3 N, float3 T, float3 B, float3 normalFromMap)
{
    T = normalize(T - N * dot(T, N));
    B = normalize(cross(T, N));

    float3x3 TBN = float3x3(T, B, N);
    return normalize(mul(normalFromMap, TBN));
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 ComputeLight(float3 N, float3 V, float3 F0, float3 L, float3 H, float distance, float3 lightColor, float metallic, float3 albedo, float roughness, float lightIntensity)
{

    // Calculate per-light radiance
   
    float attenuation = 1.0 / (distance * distance) * lightIntensity;
    float3 radiance = lightColor * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    float3 kS = F;
    float3 kD = float3(1.0, 1.0, 1.0) - kS;
    kD *= 1.0 - metallic;

    float3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    float3 specular = numerator / max(denominator, 0.001);

    // Scale light based on incident light angle relative to normal
    float NdotL = max(dot(N, L), 0.0);

    // Accumulate radiance
   return (kD * albedo / PI + specular) * radiance * NdotL;

}


float3 ComputeLightToon(float3 N, float3 V, float3 F0, float3 L, float3 H, float distance, float3 lightColor, float metallic, float3 albedo, float roughness, float lightIntensity)
{


    



    //
      // Calculate per-light radiance

    float attenuation = 1.0 / (distance * distance) * lightIntensity;
    float3 radiance = lightColor * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    float3 kS = F;
    float3 kD = float3(1.0, 1.0, 1.0) - kS;


    float diffuseF = max(dot(N, normalize(L)), 0.0f);
    if (diffuseF > 0.95)
        kD = float3(1.0, 1, 1) * kD;
    else if (diffuseF > 0.5)
        kD = float3(0.7, 0.7, 0.7) * kD;
    else if (diffuseF > 0.05)
        kD = float3(0.35, 0.35, 0.35) * kD;
    else
        kD = float3(0.1, 0.1, 0.1) * kD;


    kD *= 1.0 - metallic;

    float3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    float3 specular = numerator / max(denominator, 0.001);

    // Scale light based on incident light angle relative to normal
    float NdotL = max(dot(N, L), 0.0);

    // Accumulate radiance
    return (kD * albedo / PI + specular) * radiance * NdotL;

}



float4 CEL(float4 worldPos, float4 normal)
{
    normalize(normal.xyz);
    return worldPos += (g_lineThickness * normal);

}

//Intersection tests
struct Plane
{
    float3 N;   // Plane normal.
    float  d;   // Distance to origin.
};

struct Sphere
{
    float3 c;   // Center point.
    float  r;   // Radius.
};

struct Cone
{
    float3 T;   // Cone tip.
    float  h;   // Height of the cone.
    float3 d;   // Direction of the cone.
    float  r;   // bottom radius of the cone.
};
struct Frustum
{
    Plane planes[4];   // left, right, top, bottom frustum planes.
};
Plane ComputePlane(float3 p0, float3 p1, float3 p2)
{
    Plane plane;

    float3 v0 = p1 - p0;
    float3 v2 = p2 - p0;

    //plane.N = normalize(cross(v0, v2));
    plane.N = normalize(cross(v2, v0));
    // Compute the distance to the origin using p0.
    plane.d = dot(plane.N, p0);

    return plane;
}

// Check to see if a sphere is fully behind (inside the negative halfspace of) a plane.
// Source: Real-time collision detection, Christer Ericson (2005)
bool SphereInsidePlane(Sphere sphere, Plane plane)
{
    return dot(plane.N, sphere.c) - plane.d < -sphere.r;
   
}

// Check to see if a point is fully behind (inside the negative halfspace of) a plane.
bool PointInsidePlane(float3 p, Plane plane)
{
    return dot(plane.N, p) - plane.d < 0;
}

// Check to see if a cone if fully behind (inside the negative halfspace of) a plane.
// Source: Real-time collision detection, Christer Ericson (2005)
bool ConeInsidePlane(Cone cone, Plane plane)
{
    // Compute the farthest point on the end of the cone to the positive space of the plane.
    float3 m = cross(cross(plane.N, cone.d), cone.d);
    float3 Q = cone.T + cone.d * cone.h - m * cone.r;

    // The cone is in the negative halfspace of the plane if both
    // the tip of the cone and the farthest point on the end of the cone to the 
    // positive halfspace of the plane are both inside the negative halfspace 
    // of the plane.
    return PointInsidePlane(cone.T, plane) && PointInsidePlane(Q, plane);
}

// Check to see of a light is partially contained within the frustum.
bool SphereInsideFrustum(Sphere sphere, Frustum frustum, float zNear, float zFar)
{
  
    bool result = true;

    if (sphere.c.z - sphere.r > zFar  || sphere.c.z + sphere.r < zNear)

    {
        result = false;
    }

    // Then check frustum planes
    for (int i = 0; i < 4 && result; i++)
    {
        if (SphereInsidePlane(sphere, frustum.planes[i]))
        {
            result = false;
        }
    }

    return result;

    

 


}

bool ConeInsideFrustum(Cone cone, Frustum frustum, float zNear, float zFar)
{
    bool result = true;

    Plane nearPlane = { float3(0, 0, -1), -zNear };
    Plane farPlane = { float3(0, 0, 1), zFar };

    // First check the near and far clipping planes.
    if (ConeInsidePlane(cone, nearPlane) || ConeInsidePlane(cone, farPlane))
    {
        result = false;
    }

    // Then check frustum planes
    for (int i = 0; i < 4 && result; i++)
    {
        if (ConeInsidePlane(cone, frustum.planes[i]))
        {
            result = false;
        }
    }

    return result;
}
float3 IncreaseS(float r, float g, float b, float scalar)
{
    //convert to saturation
    float maximum = max(r, g);

    maximum = max(maximum, b);

    float minimum = min(r, g);

    minimum = min(minimum, b);

    float delta = maximum - minimum;

    if (maximum == 0)
    {
        return float3(r, g, b);
    }

    float s = delta / maximum;
    float h = 0;
    float v = maximum;
    if (delta == 0)
    {
        h = 0;
    }
    else if (maximum == r)
    {
        h = 60 * fmod(((g - b) / delta), 6);
    }
    else if (maximum == g)
    {
        h = 60 * (((b - r) / delta) + 2);
    }
    else if (maximum == b)
    {
        h = 60 * (((r - g) / delta) + 4);
    }

    //increase
    s *= scalar;
    //revert back and return

    saturate(s);
    saturate(v);

    float c = v * s;
    float x = c * (1 - fmod((h / 60), (2 - 1)));
    float m = v - c;

    if (h >= 0 && h < 60)
    {
        return float3(c, x, 0);
    }
    if (h >= 60 && h < 120)
    {
        return float3(x, c, 0);
    }
    if (h >= 120 && h < 180)
    {
        return float3(0, c, x);
    }
    if (h >= 180 && h < 240)
    {
        return float3(0, x, c);
    }
    if (h >= 240 && h < 300)
    {
        return float3(x, 0, c);
    }
    if (h >= 300 && h < 360)
    {
        return float3(c, 0, x);
    }

    return float3(r, g, b);
}


float GetL(float3 color)
{
    float RedGreenBlue[3];

    RedGreenBlue[0] = color.r;
    RedGreenBlue[1] = color.g;
    RedGreenBlue[2] = color.b;
    float smallest = 9999999;
    float biggest = 0;

    for (int i = 0; i < 3; i++)
    {
        if (RedGreenBlue[i] < smallest)
        {
            smallest = RedGreenBlue[i];
        }
        if (RedGreenBlue[i] > biggest)
        {
            biggest = RedGreenBlue[i];
        }
    }

    return (smallest + biggest) / 2.0f;
}



float GetS(float3 color)
{
    float RedGreenBlue[3];

    RedGreenBlue[0] = color.r;
    RedGreenBlue[1] = color.g;
    RedGreenBlue[2] = color.b;

    float smallest = 999999999;
    float biggest = 0;

    for (int i = 0; i < 3; i++)
    {
        if (RedGreenBlue[i] < smallest)
        {
            smallest = RedGreenBlue[i];
        }
        if (RedGreenBlue[i] > biggest)
        {
            biggest = RedGreenBlue[i];
        }
    }

    if (smallest == biggest)
    {
        return 0.0;
    }
    else
    {
        if (GetL(color) < 0.5)
        {
            return (biggest - smallest) / (biggest + smallest);
        }
        else if (GetL(color) >= 0.5)
        {
            return (biggest - smallest) / (2.0f - biggest - smallest);
        }
        else
        {
            return 0.0f; //Added this so that it will always return something.
        }
    }
}


float GetH(float3 color)
{

    float RedGreenBlue[3];

    RedGreenBlue[0] = color.r;
    RedGreenBlue[1] = color.g;
    RedGreenBlue[2] = color.b;
    float smallest = 99999999;
    float biggest = 0;
    float hue = 0.0;
    for (int i = 0; i < 3; i++)
    {
        if (RedGreenBlue[i] < smallest)
        {
            smallest = RedGreenBlue[i];
        }
        if (RedGreenBlue[i] > biggest)
        {
            biggest = RedGreenBlue[i];
        }
    }

    if (smallest == biggest)
    {

        return hue;
    }
    else
    {
        if (biggest == RedGreenBlue[0])
        {
            hue = (RedGreenBlue[1] - RedGreenBlue[2]) / (biggest - smallest);
        }
        else if (biggest == RedGreenBlue[1])
        {
            hue = 2 + (RedGreenBlue[2] - RedGreenBlue[0]) / (biggest - smallest);
        }
        else if (biggest == RedGreenBlue[2])
        {
            hue = 4 + (RedGreenBlue[0] - RedGreenBlue[1]) / (biggest - smallest);
        }

        hue *= 60;

        if (hue < 0)
        {
            hue += 360;
        }
    }

    return hue;
}


float3 PSColorMode(float3 color, float h, float s, float lum, float opacity)
{
    float3 resultRGB = float3(0, 0, 0);

    int counter = 0;
    float resultF = 0;
    float temp1 = 0;
    float temp2 = 0;
    float3 tempRGB;
    tempRGB.r = 0;
    tempRGB.g = 0;
    tempRGB.b = 0;
    if (s == 0.0)
    {
        resultRGB.r = lum;
        resultRGB.g = lum;
        resultRGB.b = lum;
    }
    else
    {
        if (lum < 0.5)
        {
            temp1 = lum * (1 + s);
        }
        else if (lum >= 0.5)
        {
            temp1 = (lum + s) - (lum * s);

        }

        temp2 = (2 * lum) - temp1;

        h /= 360.0f;

        tempRGB.r = (float)(h + 0.333);
        tempRGB.g = h;
        tempRGB.b = (float)(h - 0.333);

        if (tempRGB.r < 0)
        {
            tempRGB.r += 1;
        }
        else if (tempRGB.r > 1)
        {
            tempRGB.r -= 1;
        }
        if (tempRGB.g < 0)
        {
            tempRGB.g += 1;
        }
        else if (tempRGB.g > 1)
        {
            tempRGB.g -= 1;
        }
        if (tempRGB.b < 0)
        {
            tempRGB.b += 1;
        }
        else if (tempRGB.b > 1)
        {
            tempRGB.b -= 1;
        }

        if (6 * tempRGB.r < 1)
        {
            resultF = (temp2 + (temp1 - temp2)) * 6 * tempRGB.r;
        }
        else if (2 * tempRGB.r < 1)
        {
            resultF = temp1;
        }
        else if (3 * tempRGB.r < 2)
        {
            resultF = (temp2 + (temp1 - temp2)) * (float)((0.666 - tempRGB.r)) * 6;
        }
        else
        {
            resultF = temp2;
        }

        resultRGB.r = resultF;
        if (6 * tempRGB.g < 1)
        {
            resultF = (temp2 + (temp1 - temp2)) * 6 * tempRGB.g;
        }
        else if (2 * tempRGB.g < 1)
        {
            resultF = temp1;
        }
        else if (3 * tempRGB.g < 2)
        {
            resultF = (temp2 + (temp1 - temp2)) * (float)((0.666 - tempRGB.g)) * 6;
        }
        else
        {
            resultF = temp2;
        }

        resultRGB.g = resultF;
        if (6 * tempRGB.b < 1)
        {
            resultF = (temp2 + (temp1 - temp2)) * 6 * tempRGB.b;
        }
        else if (2 * tempRGB.b < 1)
        {
            resultF = temp1;
        }
        else if (3 * tempRGB.b < 2)
        {
            resultF = (temp2 + (temp1 - temp2)) * (float)((0.666 - tempRGB.b)) * 6;
        }
        else
        {
            resultF = temp2;
        }

        resultRGB.b = resultF;
    }

    resultRGB.r = ((1 - opacity) * color.r + (opacity * resultRGB.r));
    resultRGB.g = ((1 - opacity) * color.g + (opacity * resultRGB.g));
    resultRGB.b = ((1 - opacity) * color.b + (opacity * resultRGB.b));

    return resultRGB;
}




float3 Overlay(float3 a, float3 b)
{
    float3 grey = float3(0.5, 0.5, 0.5);
    float3 white = float3(1, 1, 1);
    float3 result = float3(0, 0, 0);

    if (b.r + b.g + b.b <= grey.r + grey.g + grey.b)
    {
        result = 2 * a * b;
    }
    else if (b.r + b.g + b.b > grey.r + grey.g + grey.b)
    {
        result = white - (2 * (white - a) * (white - b));
    }

    return result;
}


float3 Screen(float3 a, float3 b)
{
    float3 white = float3(1, 1, 1);
    return white - (white - a) * (white - b);
}


float3 LowerOpacity(float3 a, float3 b, float scalar)
{
    b.r = ((1 - scalar) * a.r + (scalar * b.r));
    b.g = ((1 - scalar) * a.g + (scalar * b.g));
    b.b = ((1 - scalar) * a.b + (scalar * b.b));

    return b;
}

float calcShadowF(float4 shadowPos, SamplerComparisonState comparison,  Texture2D shadowMap) 
{


    shadowPos.xyz /= shadowPos.w;

    float depth = shadowPos.z;

    // Texel size.
    const float dx = 1.0f / 1024.0f; //shadow map size is 1024

    float percentLit = 0.0f;

    const float2 offsets[9] =
    {
        float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
    };

    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += shadowMap.SampleCmpLevelZero(comparison, (shadowPos.xy + offsets[i]), depth).r; //does pcf
    }
    return percentLit /= 9.0f;


}

float calcShadowF(float3 fragToLight, SamplerComparisonState comparison, TextureCube shadowMap, SamplerState clampS, float3 viewWorld)
{
   
   

    const float dx = 1.0f; 


    const float3 sampleOffsetDirections[20] =
    {
           float3(dx,  dx,  -dx), float3(dx, -dx,  -dx), float3(-dx, -dx,  -dx), float3(-dx,  dx,  -dx),
            float3(dx,  dx, -dx), float3(dx, -dx, dx), float3(-dx, -dx, dx), float3(-dx,  dx, dx),
            float3(dx,  dx,  0), float3(dx, -dx,  0), float3(-dx, -dx,  0), float3(-dx,  dx,  0),
            float3(dx,  0,  -dx), float3(-dx,  0,  -dx), float3(dx,  0, dx), float3(-dx,  0, dx),
            float3(0,  dx,  -dx), float3(0, -dx,  -dx), float3(0, -dx, dx), float3(0,  dx, dx)
    };
    float far_plane = 25;
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float diskRadius = 0.05;
    float currentDepth = length(fragToLight);
    float viewDistance = length(viewWorld);
    for (int i = 0; i < samples; ++i)
    {
        float closestDepth = shadowMap.Sample(clampS, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if (currentDepth - bias > closestDepth)
        {
            shadow += 1.0;
        }
           
    }
    shadow /= float(samples);
    return shadow;
}

