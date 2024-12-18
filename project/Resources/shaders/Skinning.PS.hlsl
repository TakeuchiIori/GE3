#include "Skinning.hlsli"
struct Material
{
	float4 color : SV_TARGET0;
	int enableLighting;
	float4x4 uvTransform;
	float shininess;
};
struct DirectionalLight
{
	float4 color; // ���C�g�̐F
	float3 direction; // ���C�g�̌���
	float intensity; // �P�x
	int isEnableDirectionalLighting;
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
	int isEnablePointLight;
	float radius; // ���C�g�̓͂��ő勗��
	float decay; // ������
};
struct SpotLight
{
	float4 color;
	float3 position;
	float intensity;
	float3 direction;
	float distance;
	float decay;
	float cosAngle;
	float cosFalloffStart;
	int isEnableSpotLight;
};
struct PixelShaderOutput
{
	float4 color : SV_TARGET0;
   
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<PointLight> gPointLight : register(b3);
ConstantBuffer<SpotLight> gSpotLight : register(b4);

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
PixelShaderOutput main(VertexShaderOutput input)
{
	PixelShaderOutput output;

    // UV���W�ϊ��ƃe�N�X�`���T���v�����O
	float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

    // ������
	float3 finalDiffuse = float3(0.0f, 0.0f, 0.0f);
	float3 finalSpecular = float3(0.0f, 0.0f, 0.0f);

	if (gMaterial.enableLighting)
	{
        // �J���������x�N�g��
		float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);

        //===========================================================//
        //                   �f�B���N�V���i�����C�g�̌v�Z               
        //===========================================================//
        
		if (gDirectionalLight.isEnableDirectionalLighting)
		{
            // �g�U����
			float NdotL = max(dot(normalize(input.normal), -gDirectionalLight.direction), 0.0f);
			float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
			float3 diffuseDirectional = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
            // ���ʔ��� (Blinn-Phong)
			float3 halfVector = normalize(-gDirectionalLight.direction + toEye);
			float NdotH = max(dot(normalize(input.normal), halfVector), 0.0f);
			float3 specularDirectional = gDirectionalLight.color.rgb * gDirectionalLight.intensity * pow(saturate(NdotH), gMaterial.shininess);

            // �t���O�ŗL����
			if (gCamera.enableSpecular != 0)
			{
				finalSpecular += specularDirectional;
			}
			finalDiffuse += diffuseDirectional;
		}

        //===========================================================//
        //                      �|�C���g���C�g�̌v�Z               
        //===========================================================//
        
		if (gPointLight.isEnablePointLight)
		{
            // ���C�g�����x�N�g��
			float3 pointLightDirection = normalize(gPointLight.position - input.worldPosition);
            
			float distance = length(gPointLight.position - input.worldPosition); // �|�C���g���C�g�ւ̋���
			float factor = pow(saturate(-distance / gPointLight.radius + 1.0f), gPointLight.decay); //�w���ɂ��R���g���[��
            
            // �g�U����
			float NdotLPoint = max(dot(normalize(input.normal), pointLightDirection), 0.0f);
			float3 diffusePoint = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * NdotLPoint * gPointLight.intensity * factor;
            // ���ʔ��� (Blinn-Phong)
			float3 halfVectorPoint = normalize(pointLightDirection + toEye);
			float NdotHPoint = max(dot(normalize(input.normal), halfVectorPoint), 0.0f);
			float3 specularPoint = gPointLight.color.rgb * gPointLight.intensity * pow(saturate(NdotHPoint), gMaterial.shininess) * factor;
        
            // �t���O�ŗL����
			if (gCamera.enableSpecular != 0)
			{
				finalSpecular += specularPoint;
			}
			finalDiffuse += diffusePoint;
		}
        
        //===========================================================//
        //                      �X�|�b�g���C�g�̌v�Z               
        //===========================================================//
        
		if (gSpotLight.isEnableSpotLight)
		{
            // �X�|�b�g���C�g�̕����x�N�g��
			float3 spotLightDirectionOnSurface = normalize(input.worldPosition - gSpotLight.position);

            // ���C�g�Ƃ̋���
			float distanceToSurface = length(gSpotLight.position - input.worldPosition);

            // �����ɂ�錸���� (�t�����̌����v�Z)
			float distanceDecay = pow(saturate(-distanceToSurface / gSpotLight.distance + 1.0f), gSpotLight.decay);

            // �X�|�b�g���C�g�̊p�x�ɂ�錸�� (Falloff�J�n�p�x���l��)
			float cosAngle = dot(spotLightDirectionOnSurface, gSpotLight.direction);
			float angleFalloff = saturate((cosAngle - gSpotLight.cosFalloffStart) / (gSpotLight.cosAngle - gSpotLight.cosFalloffStart));

            // �������W��
			float falloffFactor = angleFalloff * distanceDecay;

            // �g�U���� (NdotL)
			float NdotLPoint = max(dot(normalize(input.normal), -spotLightDirectionOnSurface), 0.0f);
			float3 diffusePoint = gMaterial.color.rgb * textureColor.rgb * gSpotLight.color.rgb * NdotLPoint * gSpotLight.intensity * falloffFactor;

            // ���ʔ��� (Blinn-Phong)
			float3 halfVectorPoint = normalize(-spotLightDirectionOnSurface + toEye);
			float NdotHPoint = max(dot(normalize(input.normal), halfVectorPoint), 0.0f);
			float3 specularPoint = gSpotLight.color.rgb * gSpotLight.intensity * pow(saturate(NdotHPoint), gMaterial.shininess) * falloffFactor;

            // �X�y�L�����[���˂̗L����
			if (gCamera.enableSpecular != 0)
			{
				finalSpecular += specularPoint;
			}
			finalDiffuse += diffusePoint;
		}

        // �ŏI�I�ȐF������
		output.color.rgb = finalDiffuse + finalSpecular;
	}
	else
	{
        // ���C�e�B���O�Ȃ��̏ꍇ
		output.color.rgb = gMaterial.color.rgb * textureColor.rgb;
	}

    // �A���t�@�l�̐ݒ�
	output.color.a = gMaterial.color.a * textureColor.a;

	return output;
}
