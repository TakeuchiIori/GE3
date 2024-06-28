#pragma once
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
class ResourceObject
{
public: 
	ResourceObject(ID3D12Resource* resource)
		:resource_(resource)
	{}
	// デストラクタはオブジェクトの寿命が尽きたときに呼ばれる
	~ResourceObject() {
		if (resource_) {
			resource_->Release();
		}
	}
	ID3D12Resource* Get() { return resource_; }
	// ここでReleaseを呼ぶ

private:
	ID3D12Resource* resource_;
};

