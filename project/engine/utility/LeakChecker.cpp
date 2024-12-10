#include "LeakChecker.h"
D3DResourceLeakChecker* D3DResourceLeakChecker::GetInstance() {
	static D3DResourceLeakChecker instance;
	return &instance;
}