cbuffer param : register(b0)
{
    float4x4 matWorldViewProj;
    float4x4 matWorld;
    float4 vLumiere;
    float4 vCamera;
    float4 vAEcl;
    float4 vAMat;
    float4 vDEcl;
    float4 vDMat;
    float4 vSEcl;
    float4 vSMat;
    float puissance;
    int bTex;
    float2 remplissage;
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
    float3 R = normalize(2 * dot(N, L) * N - L);

    float diff = saturate(dot(N, L));
    float spec = pow(saturate(dot(R, V)), puissance);

    float3 couleur;

    if (bTex > 0)
    {
        float3 texColor = textureEntree.Sample(SampleState, vin.coordTex).rgb;
        couleur = texColor * (vAEcl.rgb + vDEcl.rgb * diff) + vSEcl.rgb * vSMat.rgb * spec;
    }
    else
    {
        couleur = vAEcl.rgb * vAMat.rgb +
                  vDEcl.rgb * vDMat.rgb * diff +
                  vSEcl.rgb * vSMat.rgb * spec;
    }

    return float4(couleur, 1.0f);
}

