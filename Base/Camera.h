#pragma once
#include "../Core/Graphics.h"

class Camera
{
public:
	Camera() = default;

	tn::Vector3 ViewportPosition = { 0.0f, 0.0f, 0.0f };
	tn::Vector2 ViewportSize = { 0.0f, 0.0f };
	tn::Vector2 ViewportOffset = { 0.0f, 0.0f };
	tn::Color3 BackgroundColor = { 0,0,0 };

	float NearView = 0.0f;
	float FarView = 10.0f;
	
	void Release()
	{
		delete this;
	};
private:
};

