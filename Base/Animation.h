#pragma once
#include <Windows.h>

enum AnimationValueType
{
	Float = 0,
	Double = 1,
};

enum AnimationMovementType
{
	Straight = 0,
	Square = 1,
	Sine = 2,
};

enum AnimationMovementInOrOut
{
	In = 0,
	Out = 1,
	Both = 2,
};

struct AnimationValue
{
	double DoubleNum = 0;
	float FloatNum = 0;
};

class Animation
{
public:
	Animation();
	Animation(const Animation& val);
	Animation(const Animation* const p_val);

	AnimationValueType ValueType = AnimationValueType::Float;
	AnimationMovementType MovementType = AnimationMovementType::Straight;
	AnimationMovementInOrOut InOrOut = AnimationMovementInOrOut::In;
	void* TargetProperty = nullptr;

	float Duration = 0.0f; //Seconds

	void Play();
	void Pause();

	void SetStart(void* Value);
	void SetEnd(void* Value);

	void WaitUntilCompleted();
	bool IsEnded();
private:
	friend class Program;

	AnimationValue target = {};
	AnimationValue start = {};
	AnimationValue end = {};

	bool IsPlaying = false;
	bool IsPaused = false;
	bool Completed = false;
	float CurrentTime = 0.0f;

	void AddStep(float Time);

	struct DataTypeConversionLerpS {
		Animation* base = nullptr;
		void Double(double t);
		void Float(float t);
	} DataTypeConversionLerp;

	void Lerp(float t);

	struct DataTypeConversionSquareS {
		Animation* base = nullptr;
		void Double(double t);
		void Float(float t);
	} DataTypeConversionSquare;

	void Square(float t);

	struct DataTypeConversionSineS {
		Animation* base = nullptr;
		void Double(float t);
		void Float(float t);
	} DataTypeConversionSine;

	void Sine(float t);

	float EaseOutQuad(float t) {
		return 1 - (1 - t) * (1 - t);
	};

	Animation* ProgramDataLinkedAnimation = nullptr;
};