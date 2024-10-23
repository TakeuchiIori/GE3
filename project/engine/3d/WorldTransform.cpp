#include <WorldTransform.h>
#include <MathFunc.h>
#include <DirectXCommon.h>
#include <Object3dCommon.h>

void WorldTransform::Initialize()
{
    // ワールド行列の初期化
    matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);

    // 定数バッファ生成
    CreateConstBuffer();

    // 定数バッファへ初期行列を転送
    TransferMatrix();
}

void WorldTransform::CreateConstBuffer()
{
    // MVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
    constBuffer_ = Object3dCommon::GetInstance()->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));
    // 書き込むためのアドレスを取得
    constBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&constMap_));
    // 単位行列を書き込んでおく
    constMap_->WVP = MakeIdentity4x4();
    constMap_->World = MakeIdentity4x4();
    constMap_->WorldInverse = MakeIdentity4x4();
}

void WorldTransform::TransferMatrix()
{
    // スケール、回転、平行移動を合成して行列を計算する
    matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);

    // ワールド行列を定数バッファに転送
    if (constMap_ != nullptr) {
        // 親が存在する場合、親のワールド行列を掛け合わせる
        if (parent_) {
            Matrix4x4 parentMatrix = parent_->matWorld_;
            matWorld_ = matWorld_ * parentMatrix; // 親の行列と自身の行列を合成
        }

        constMap_->World = matWorld_; // 定数バッファに行列をコピー
        constMap_->WorldInverse = Inverse(matWorld_);
    }
}
