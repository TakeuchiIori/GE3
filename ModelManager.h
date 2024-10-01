#pragma once
class ModelManager
{



private:
	static ModelManager* instance;
	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(ModelManager&) = delete;
	ModelManager& operator = (ModelManager&) = delete;
};

