#include "Line.hlsli"

struct LinePixelOutput
{
    float4 color : SV_TARGET; // 出力色
};

LinePixelOutput main(LineVertexOutput input)
{
    LinePixelOutput output;
    // 補間された色をそのまま使用
    output.color = input.color;
    return output;
}
