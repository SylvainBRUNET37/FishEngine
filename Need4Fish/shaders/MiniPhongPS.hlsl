// Per-frame: Camera and light data
cbuffer FrameBuffer : register(b0)
{
    float4x4 matViewProj;
    float4 vLumiere; // xyz = light position
    float4 vCamera; // xyz = camera position
    float4 vAEcl; // ambient light color
    float4 vDEcl; // diffuse light color
    float4 vSEcl; // specular light color
};

// Per-object: Transform
cbuffer ObjectBuffer : register(b1)
{
    float4x4 matWorld;
};

// Per-material: Material parameters
cbuffer MaterialBuffer : register(b2)
{
    float4 vAMat; // ambient color
    float4 vDMat; // diffuse color
    float4 vSMat; // specular color
    float puissance; // shininess
    float bTex; // texture enabled (float for alignment)
    float2 padding; // alignment padding
};

Texture2D textureEntree : register(t0);
SamplerState SampleState : register(s0);

struct VSOutput
{
    float4 Pos : SV_Position;
    float3 Norm : TEXCOORD0;
    float3 vDirLum : TEXCOORD1;
    float3 vDirCam : TEXCOORD2;
    float2 coordTex : TEXCOORD3;
};

float4 MiniPhongPS(VSOutput vin) : SV_Target
{
    float3 N = normalize(vin.Norm);
    float3 L = normalize(vin.vDirLum);
    float3 V = normalize(vin.vDirCam);
    float3 R = reflect(-L, N);

    float diff = saturate(dot(N, L));
    float spec = pow(saturate(dot(R, V)), puissance);

    float3 ambient = vAEcl.rgb * vAMat.rgb;
    float3 diffuse = vDEcl.rgb * vDMat.rgb * diff;
    float3 specular = vSEcl.rgb * vSMat.rgb * spec;

    float3 color = ambient + diffuse + specular;

    if (bTex > 0.5f)
    {
        float3 texColor = textureEntree.Sample(SampleState, vin.coordTex).rgb;
        color *= texColor;
    }

    return float4(color, 1.0f);
}
