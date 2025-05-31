#pragma once
#include "../Core/Object.h"
#include "Classes.h"
#include <Windows.h>

class ObjArray
{
public:
	bool EnableGradualCleanup = false;
	ObjArray() {
		Base = new Object * [1];
		Base[0] = nullptr;
	}
	ObjArray(const ObjArray& tocopy) {
		this->Base = new Object * [tocopy.max];
		AsReference = true;
		this->max = tocopy.max;
		for (size_t i = 0; i < max; ++i) {
			this->Base[i] = tocopy.Base[i];
		}
	}
	~ObjArray() {
		if (Base)
		{
			if (AsReference == false)
			{
				for (size_t i = 0; i < max; ++i) {
					if (Base[i] != nullptr)
					{
						Base[i]->Release();
						Base[i] = nullptr;
					}
					if (EnableGradualCleanup == true)
					{
						Sleep(100);
					}
				}
			}
			delete[] Base;
			Base = nullptr;
		}
	};

	Object* operator[](size_t i) {
		if (i < this->max)
		{
			return Base[i];
		}
		else
		{
			return nullptr;
		}
	};

	void Add(Object* obj)
	{
		bool EmptyFound = false;
		size_t index = 0;
		for (size_t i = 0; i < max; ++i) {
			if (Base[i] == obj)
			{
				return;
			}

			if (Base[i] == nullptr)
			{
				index = i;
				EmptyFound = true;
			}
		}

		if (EmptyFound == true)
		{
			Base[index] = obj;
		}
		else
		{
			Expand();
			Base[max - 1] = obj;
		}

		childamount++;
	};

	void Remove(Object* obj)
	{
		bool EmptyFound = false;
		size_t index = 0;
		for (size_t i = 0; i < max; ++i) {
			if (Base[i] == obj)
			{
				Base[i] = nullptr;
				childamount--;
				this->ShrinkToFit();
				break;
			}
		}
	};

	void Swap(Object* obj1, Object* obj2)
	{
		bool Obj1Found = false;
		bool Obj2Found = false;
		size_t index1 = 0;
		size_t index2 = 0;
		for (size_t i = 0; i < max; ++i) {
			if (Obj1Found == false)
			{
				if (Base[i] == obj1)
				{
					Obj1Found = true;
					index1 = i;
				}
			}

			if (Obj2Found == false)
			{
				if (Base[i] == obj2)
				{
					Obj2Found = true;
					index2 = i;
				}
			}

			if (Obj1Found == true and Obj2Found == true) {
				Object* obj1 = Base[index1];
				Base[index1] = Base[index2];
				Base[index2] = obj1;
				break;
			};
		}
	}

	size_t Size()
	{
		return this->max;
	}
private:
	friend class GUIFolder;
	friend class ProgramObject;
	void Expand()
	{
		Object** OldBase = Base;
		size_t oldmax = max;
		max += 1;
		Base = new Object * [max];
		Base[max - 1] = nullptr;
		for (size_t i = 0; i < oldmax; ++i) {
			Base[i] = OldBase[i];
		}
		delete[] OldBase;
	};

	void ShrinkToFit()
	{
		size_t emptycount = 0;
		for (size_t i = 0; i < max; ++i) {
			if (Base[i] == nullptr)
			{
				emptycount++;
			}
		}

		if (emptycount > 0)
		{
			Object** OldBase = Base;
			size_t oldmax = max;
			max -= emptycount;
			Base = new Object * [max];

			size_t newIndex = 0;
			for (size_t i = 0; i < oldmax; ++i) {
				if (OldBase[i] != nullptr) {
					Base[newIndex++] = OldBase[i];
				}
			}

			delete[] OldBase;
		}
	};

	size_t max = 1;
	size_t childamount = 0;
	Object** Base = nullptr;
	bool AsReference = false;
};

class ProgramObject : public Object
{
public:
	ProgramObject(Program* m_Program);
	~ProgramObject();

	bool IsActive();
	virtual void Destroy();

	virtual void SetParent(ProgramObject* Parent);
	virtual ProgramObject* GetParent();

	virtual void AddChild(Object* Child);
	virtual void RemoveChild(Object* Child);
	Object* GetChild(std::wstring Name);
	Object* WaitForChild(std::wstring Name);
	virtual ObjArray GetChildren(size_t* AmountOfChildren = nullptr);
	virtual std::vector<Object*> GetChildrenVector();
	std::vector<Object*> GetChildrenWithClassVector(ObjectClass Class);
	virtual Object** GetDescendants(size_t* AmountOfDescendants = nullptr);
	virtual void Release() override; //UNSAFE

	void EnableGradualCleanupWhenDestroyed();
	virtual ObjectClass GetType() { 
		return ObjectClass::ProgramObject;
	};

protected:
	friend class Program;

	bool Active = false;
	Program* m_Program = nullptr;
	ProgramObject* Parent = nullptr;

	ObjArray* Children = nullptr;
};