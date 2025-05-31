#include "Object.h"
#include <Windows.h>

Object::Object()
{

}

Object::~Object()
{

}

void Object::SetName(std::wstring newname)
{
	this->name = newname;
}

std::wstring Object::GetName()
{
	return this->name;
}

void Object::Update()
{

}

unsigned long Object::AddRef() {
	this->Refs++;
	return Refs;
}

void Object::Release()
{
	if (Refs > 1) {
		--this->Refs;
	}
	else {
		delete this;
	}
}