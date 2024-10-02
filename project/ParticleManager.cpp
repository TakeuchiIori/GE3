#include "ParticleManager.h"

ParticleManager* ParticleManager::GetInstance()
{
    static ParticleManager instance;
    return &instance;
}

void ParticleManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager)
{
    // ポインタを渡す
    this->dxCommon_ = dxCommon;
    this->srvManager_ = srvManager;
}

void ParticleManager::Update()
{


}
