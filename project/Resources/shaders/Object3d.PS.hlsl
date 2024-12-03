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
    int enablePointLight;
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

    // UV座標変換とテクスチャサンプリング
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

    // 初期化
    float3 finalDiffuse = float3(0.0f, 0.0f, 0.0f);
    float3 finalSpecular = float3(0.0f, 0.0f, 0.0f);

    if (gMaterial.enableLighting != 0)
    {
        // カメラ視線ベクトル
        float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);

        // ディレクショナルライトの計算
        if (gDirectionalLight.intensity > 0.0f)
        {
            // 拡散反射
            float NdotL = max(dot(normalize(input.normal), -gDirectionalLight.direction), 0.0f);
            float3 diffuseDirectional = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * NdotL * gDirectionalLight.intensity;

            // 鏡面反射 (Blinn-Phong)
            float3 halfVector = normalize(-gDirectionalLight.direction + toEye);
            float NdotH = max(dot(normalize(input.normal), halfVector), 0.0f);
            float3 specularDirectional = gDirectionalLight.color.rgb * gDirectionalLight.intensity *
                                          pow(NdotH, gMaterial.shininess);

            // フラグで有効化
            if (gCamera.enableSpecular != 0)
            {
                finalSpecular += specularDirectional;
            }
            finalDiffuse += diffuseDirectional;
        }

        // ポイントライトの計算
        if (gPointLight.enablePointLight != 0 && gPointLight.intensity > 0.0f)
        {
            // ライト方向ベクトル
            float3 pointLightDirection = normalize(gPointLight.position - input.worldPosition);

            // 拡散反射
            float NdotLPoint = max(dot(normalize(input.normal), pointLightDirection), 0.0f);
            float3 diffusePoint = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * NdotLPoint * gPointLight.intensity;

            // 鏡面反射 (Blinn-Phong)
            float3 halfVectorPoint = normalize(pointLightDirection + toEye);
            float NdotHPoint = max(dot(normalize(input.normal), halfVectorPoint), 0.0f);
            float3 specularPoint = gPointLight.color.rgb * gPointLight.intensity *
                                   pow(NdotHPoint, gMaterial.shininess);

            // フラグで有効化
            if (gCamera.enableSpecular != 0)
            {
                finalSpecular += specularPoint;
            }
            finalDiffuse += diffusePoint;
        }

        // 最終的な色を合成
        output.color.rgb = finalDiffuse + finalSpecular;
    }
    else
    {
        // ライティングなしの場合
        output.color.rgb = gMaterial.color.rgb * textureColor.rgb;
    }

    // アルファ値の設定
    output.color.a = gMaterial.color.a * textureColor.a;

    return output;
}
