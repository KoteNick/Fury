#pragma once
#include "Core/Entity.h"
#include <vector>

class Game {
	struct WindowSize {
		int width, height;
	};
private:
	static Game* instance;
public:
	Game() {
		instance = this;
	}
	~Game() {
		instance = nullptr;
	}
	static Game& Get() { return *instance; }
public:
	float time = 0;
	WindowSize windowSize = {};
	Entity triangle, triangle2, camera, cube;

	void Init();
	void OnUpdate(float deltaTime);
	void Close();
};