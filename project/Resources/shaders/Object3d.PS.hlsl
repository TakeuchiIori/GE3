#include "Object3d.hlsli"
struct Material{
    float4 color:SV_TARGET0;
    int enableLighting;
    float4x4 uvTransform;
    float shininess;
};
struct DirectionalLight
{
    float4 color;       // ライトの色
    float3 direction;   // ライトの向き
    float intensity;    // 輝度
};
struct Camera
{
    float3 worldPosition;
};
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
   
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    // 鏡面反射
    float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    float3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
    float RdoE = dot(reflectLight, toEye);
    float specularPow = pow(saturate(RdoE), gMaterial.shininess); // 反射強度
    
    if (gMaterial.enableLighting != 0)
    {
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        
        // 拡散反射
        float3 diffuse =
        gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        // 鏡面反射
        float3 specular =                                                         // 物体の鏡面反射色
        gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float3(0.0f, 0.0f, 0.0f);
        
        output.color.rgb = diffuse + specular;
        
        output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
        // アルファ値の確認用
        output.color.a = gMaterial.color.a * textureColor.a; // アルファ値の掛け合わせ
    }
    else
    {
        output.color = gMaterial.color * textureColor;
        output.color.a = gMaterial.color.a * textureColor.a; // アルファ値の掛け合わせ
    }

    return output;
}
