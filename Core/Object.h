#pragma once
#include <vector>
#include <string>

#define TN_ATTRIBUTE_DOES_NOT_EXIST -1
#define TN_ATTRIBUTE_ALREADY_EXIST -2

class Program;
class Scene;

class Object
{
public:
	Object();
	~Object();

	virtual void SetName(std::wstring newname);
	virtual std::wstring GetName();

	virtual void Update();
	virtual unsigned long AddRef();

	virtual void Release(); //UNSAFE
protected:
	friend class Scene;
	friend class Program;

	std::wstring name = L"Object";

	size_t Refs = 1;
};