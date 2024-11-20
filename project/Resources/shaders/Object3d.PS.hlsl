#include "Object3d.hlsli"
struct Material
{
    float4 color : SV_TARGET0;
    int enableLighting;
    float4x4 uvTransform;
    float shininess;
};
struct DirectionalLight
{
    float4 color; // ライトの色
    float3 direction; // ライトの向き
    float intensity; // 輝度
};
struct Camera
{
    float3 worldPosition;
    int enableSpecular;
    int isHalfVector;
};
struct PointLight
{
    float4 color;
    float3 position;
    float intensity;
};
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
   
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<PointLight> gPointLight : register(b3);

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

    if (gMaterial.enableLighting != 0)
    {
        // 拡散反射の計算
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        float3 diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        // 鏡面反射の計算
        float3 specular = float3(0.0f, 0.0f, 0.0f);
        // 鏡面反射のON : OFF
        if (gCamera.enableSpecular != 0)
        {
            float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
            float3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
            float RdoE = dot(reflectLight, toEye);
            float specularPow = pow(saturate(RdoE), gMaterial.shininess);
            
            if (gCamera.isHalfVector != 0)
            {
                float3 halfVector = normalize(-gDirectionalLight.direction + toEye);
                float NdotH = dot(normalize(input.normal), halfVector);
                specularPow = pow(saturate(NdotH), gMaterial.shininess);
            }
            
            specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
        }

        // 拡散反射 + 鏡面反射
        output.color.rgb = diffuse + specular;

        // アルファ値
        output.color.a = gMaterial.color.a * textureColor.a; // アルファ値の掛け合わせ
    }
    else
    {
        // ライティングなしの場合
        output.color = gMaterial.color * textureColor;
        output.color.a = gMaterial.color.a * textureColor.a; // アルファ値の掛け合わせ
    }

    return output;
}