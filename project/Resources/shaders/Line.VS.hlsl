#include "Line.hlsli"

struct TransformationMatrix
{
    float4x4 WVP; // ワールド・ビュー・プロジェクション行列
    float4x4 World; // ワールド行列（必要なら）
};

struct LineVertexOutput
{
    float4 position : SV_POSITION; // 変換後のスクリーン座標
    float4 color : COLOR; // 頂点の色（補間される）
};
// 定数バッファ
ConstantBuffer<TransformationMatrix> gTransform : register(b0);

LineVertexOutput main(LineVertexInput input)
{
    LineVertexOutput output;
    // ワールド・ビュー・プロジェクション行列を使って座標を変換
    output.position = mul(float4(input.position, 1.0f), gTransform.WVP);
    // 頂点の色をそのまま出力
    output.color = input.color;
    return output;
}
