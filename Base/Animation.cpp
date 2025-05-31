#include "Animation.h"
#include <Windows.h>
#include <math.h>

Animation::Animation()
{
	DataTypeConversionLerp.base = this;
	DataTypeConversionSquare.base = this;
	DataTypeConversionSine.base = this;
}

Animation::Animation(const Animation& val)
{
	this->ValueType = val.ValueType;
	this->TargetProperty = val.TargetProperty;
	this->Duration = val.Duration;
	this->Completed = false;
	this->CurrentTime = 0.0f;
	//this->ProgramDataLinkedAnimation = nullptr;
	this->start = val.start;
	this->end = val.end;
	this->IsPaused = false;
	this->DataTypeConversionLerp.base = this;
	this->DataTypeConversionSquare.base = this;
	this->DataTypeConversionSine.base = this;
}

Animation::Animation(const Animation* const p_val)
{
	this->ValueType = p_val->ValueType;
	this->TargetProperty = p_val->TargetProperty;
	this->Duration = p_val->Duration;
	this->Completed = false;
	this->CurrentTime = 0.0f;
	//this->ProgramDataLinkedAnimation = nullptr;
	this->start = p_val->start;
	this->end = p_val->end;
	this->IsPaused = false;
	this->DataTypeConversionLerp.base = this;
	this->DataTypeConversionSquare.base = this;
	this->DataTypeConversionSine.base = this;
}

void Animation::Play()
{
	if (ProgramDataLinkedAnimation != nullptr)
	{
		ProgramDataLinkedAnimation->IsPlaying = true;
		this->IsPlaying = true;
		if (ProgramDataLinkedAnimation->IsPaused == true)
		{
			ProgramDataLinkedAnimation->IsPaused = false;
		}
	}
}

void Animation::SetStart(void* Value)
{
	switch (ValueType)
	{
	case AnimationValueType::Float:
		start.FloatNum = *static_cast<float*>(Value); break;
	case AnimationValueType::Double:
		start.DoubleNum = *static_cast<double*>(Value); break;
	}
}

void Animation::SetEnd(void* Value)
{
	switch (ValueType)
	{
	case AnimationValueType::Float:
		end.FloatNum = *static_cast<float*>(Value); break;
	case AnimationValueType::Double:
		end.DoubleNum = *static_cast<double*>(Value); break;
	}
}

void Animation::AddStep(float Time)
{
	if (this->IsPlaying == true and this->IsPaused == false)
	{
		CurrentTime += Time;
		if (CurrentTime > Duration)
		{
			CurrentTime = Duration;
			this->Completed = true;
		}

		float t = CurrentTime / Duration;

		if (TargetProperty != nullptr)
		{
			switch (this->MovementType)
			{
			case AnimationMovementType::Straight:
				Lerp(t);
				break;
			case AnimationMovementType::Square:
				Square(t);
				break;
			case AnimationMovementType::Sine:
				Sine(t);
				break;
			default:
				break;
			}
		}
	}
}

void Animation::WaitUntilCompleted()
{
	if (ProgramDataLinkedAnimation != nullptr)
	{
		while (ProgramDataLinkedAnimation->Completed == false) { Sleep(1); };
	}
}

bool Animation::IsEnded()
{
	return this->Completed == true;
}

void Animation::Pause()
{
	this->IsPaused = true;
}

////////////////////////////////////////////

void Animation::DataTypeConversionLerpS::Double(double t)
{
	(*static_cast<double*>(base->TargetProperty)) = ((base->start.FloatNum) * (1.0f - t)) + ((base->end.FloatNum) * pow(t, 2));
}

void Animation::DataTypeConversionLerpS::Float(float t)
{
	(*static_cast<float*>(base->TargetProperty)) = ((base->start.FloatNum) * (1.0f - t)) + ((base->end.FloatNum) * t);
}


void Animation::Lerp(float t)
{
	switch (ValueType)
	{
	case AnimationValueType::Float:
		this->DataTypeConversionLerp.Float(t);
		break;
	case AnimationValueType::Double:
		this->DataTypeConversionLerp.Double(static_cast<double>(t));
		break;
	}
}

void Animation::DataTypeConversionSquareS::Double(double t)
{
	double curvedT = 0.0;
	switch (base->InOrOut)
	{
	case AnimationMovementInOrOut::In:
		curvedT = t * t;
		(*static_cast<double*>(base->TargetProperty)) = ((base->start.FloatNum) * (1.0f - curvedT)) + ((base->end.FloatNum) * curvedT);
		break;
	case AnimationMovementInOrOut::Out:
		curvedT = 1.0 - (1.0 - t) * (1.0 - t);
		(*static_cast<double*>(base->TargetProperty)) = ((base->start.FloatNum) * (1.0f - curvedT)) + ((base->end.FloatNum) * curvedT);
		break;
	case AnimationMovementInOrOut::Both:
	{
		curvedT;
		if (t < 0.5f) {
			curvedT = 2 * t * t;
		}
		else {
			curvedT = -1 + (4 - 2 * t) * t;
		}
		(*static_cast<double*>(base->TargetProperty)) = ((base->start.FloatNum) * (1.0f - curvedT)) + ((base->end.FloatNum) * curvedT);
		break;
	}
	}
}

void Animation::DataTypeConversionSquareS::Float(float t)
{
	float curvedT = 0.0f;
	switch (base->InOrOut)
	{
	case AnimationMovementInOrOut::In:
		curvedT = t * t;
		(*static_cast<float*>(base->TargetProperty)) = ((base->start.FloatNum) * (1.0f - curvedT)) + ((base->end.FloatNum) * curvedT);
		break;
	case AnimationMovementInOrOut::Out:
		curvedT = 1.0f - (1.0f - t) * (1.0f - t);
		(*static_cast<float*>(base->TargetProperty)) = ((base->start.FloatNum) * (1.0f - curvedT)) + ((base->end.FloatNum) * curvedT);
		break;
	case AnimationMovementInOrOut::Both:
	{
		curvedT;
		if (t < 0.5f) {
			curvedT = 2 * t * t;
		}
		else {
			curvedT = -1 + (4 - 2 * t) * t;
		}
		(*static_cast<float*>(base->TargetProperty)) = ((base->start.FloatNum) * (1.0f - curvedT)) + ((base->end.FloatNum) * curvedT);
		break;
	}
	}
}

void Animation::Square(float t)
{
	switch (ValueType)
	{
	case AnimationValueType::Float:
		this->DataTypeConversionSquare.Float(t);
		break;
	case AnimationValueType::Double:
		this->DataTypeConversionSquare.Double(static_cast<double>(t));
		break;
	}
}

void Animation::DataTypeConversionSineS::Double(float t)
{
	double curvedT = 0.0;
	switch (base->InOrOut)
	{
	case AnimationMovementInOrOut::In:
		curvedT = sin(t * 3.14156) / 2;
		(*static_cast<double*>(base->TargetProperty)) = ((base->start.FloatNum) * (1.0f - curvedT)) + ((base->end.FloatNum) * curvedT);
		break;
	case AnimationMovementInOrOut::Out:
		curvedT = 1.0 - (sin(t * 3.14156) / 2);
		(*static_cast<double*>(base->TargetProperty)) = ((base->start.FloatNum) * (1.0f - curvedT)) + ((base->end.FloatNum) * curvedT);
		break;
	case AnimationMovementInOrOut::Both:
	{
		curvedT;
		if (t < 0.5f) {
			curvedT = 2 * t * t;
		}
		else {
			curvedT = -1 + (4 - 2 * t) * t;
		}
		(*static_cast<double*>(base->TargetProperty)) = ((base->start.FloatNum) * (1.0f - curvedT)) + ((base->end.FloatNum) * curvedT);
		break;
	}
	}
}

void Animation::DataTypeConversionSineS::Float(float t)
{
	float curvedT = 0.0f;
	switch (base->InOrOut)
	{
	case AnimationMovementInOrOut::In:
		curvedT = sinf(t * 3.14156f) / 2;
		(*static_cast<float*>(base->TargetProperty)) = ((base->start.FloatNum) * (1.0f - curvedT)) + ((base->end.FloatNum) * curvedT);
		break;
	case AnimationMovementInOrOut::Out:
		curvedT = 1.0f - (sinf(t * 3.14156f) / 2);
		(*static_cast<float*>(base->TargetProperty)) = ((base->start.FloatNum) * (1.0f - curvedT)) + ((base->end.FloatNum) * curvedT);
		break;
	case AnimationMovementInOrOut::Both:
	{
		if (t < 0.5f) {
			curvedT = sinf(t * 3.14156f) / 2;
		}
		else {
			curvedT = 1.0f - (sinf(t * 3.14156f) / 2);
		}
		(*static_cast<float*>(base->TargetProperty)) = ((base->start.FloatNum) * (1.0f - curvedT)) + ((base->end.FloatNum) * curvedT);
		break;
	}
	}
}

void Animation::Sine(float t)
{
	switch (ValueType)
	{
	case AnimationValueType::Float:
		this->DataTypeConversionSine.Float(t);
		break;
	case AnimationValueType::Double:
		this->DataTypeConversionSine.Double(static_cast<double>(t));
		break;
	}
}