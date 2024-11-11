#include "Object3d.hlsli"
struct Material{
    float4 color:SV_TARGET0;
    int enableLighting;
    float4x4 uvTransform;
};
struct DirectionalLight
{
    float4 color;       // ライトの色
    float3 direction;   // ライトの向き
    float intensity;    // 輝度
};
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
   
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

    if (gMaterial.enableLighting != 0)
    {
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;

        // アルファ値の確認用
        output.color.a = gMaterial.color.a * textureColor.a; // アルファ値の掛け合わせ
    }
    else
    {
        output.color = gMaterial.color * textureColor;
        output.color.a = gMaterial.color.a * textureColor.a; // アルファ値の掛け合わせ
    }
    
    if (output.color.a == 0.0)
    {
        discard;
    }
   
    return output;
}
