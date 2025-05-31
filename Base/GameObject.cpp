#include "GameObjectBase.h"
#include "GameObject.h"
#include "Base.h"

ProgramObject::ProgramObject(Program* m_Program)
{
	this->Children = new ObjArray();
	this->Active = true;
	this->m_Program = m_Program;
}

ProgramObject::~ProgramObject()
{
	
}

bool ProgramObject::IsActive()
{
	return this->Active == true;
}

void ProgramObject::Destroy()
{
	this->Active = false;
	this->SetParent(nullptr);
	this->m_Program->AddObjectToGarbageCollection(this);
}

void ProgramObject::SetParent(ProgramObject* Parent)
{
	if (this->Parent != nullptr)
	{
		if (ProgramObject* po_parent = dynamic_cast<ProgramObject*>(this->Parent))
		{
			po_parent->Children->Remove(this);
		}
	}
	this->Parent = Parent;
	if (this->Parent != nullptr) //still no nullptr?
	{
		if (ProgramObject* po_parent = dynamic_cast<ProgramObject*>(this->Parent))
		{
			po_parent->Children->Add(this);
		}
	}
}

ProgramObject* ProgramObject::GetParent()
{
	return this->Parent;
}

void ProgramObject::AddChild(Object* Child)
{
	if (Child != nullptr)
	{
		if (ProgramObject* po_child = dynamic_cast<ProgramObject*>(Child))
		{
			po_child->SetParent(this);
		}
		else
		{
			this->Children->Add(Child);
		}
	}
}

void ProgramObject::RemoveChild(Object* Child)
{
	if (Child != nullptr)
	{
		this->Children->Remove(Child);
	}
}

Object* ProgramObject::GetChild(std::wstring Name)
{
	for (size_t i = 0; i < this->Children->childamount; i++)
	{
		Object* Obj = (*this->Children)[i];
		if (Obj != nullptr)
		{
			if (Name.compare(Obj->GetName()) == 0)
			{
				return Obj;
			}
		}
	}

	return nullptr;
}

Object* ProgramObject::WaitForChild(std::wstring Name)
{
	Object* Obj = nullptr;
	while (Obj == nullptr)
	{
		for (size_t i = 0; i < this->Children->childamount; i++)
		{
			Object* Obj = (*this->Children)[i];
			if (Obj != nullptr)
			{
				if (Name.compare(Obj->GetName()) == 0)
				{
					return Obj;
				}
			}
		}
		Sleep(1);
	}
	return nullptr;
}

ObjArray ProgramObject::GetChildren(size_t* AmountOfChildren)
{
	if (AmountOfChildren != nullptr)
	{
		(*AmountOfChildren) = this->Children->childamount;
	}
	return ObjArray(*this->Children);
}

std::vector<Object*> ProgramObject::GetChildrenVector()
{
	std::vector<Object*> vector = {};
	for (size_t i = 0; i < this->Children->childamount; i++)
	{
		vector.push_back((*this->Children)[i]);
	}
	return vector;
}

std::vector<Object*> ProgramObject::GetChildrenWithClassVector(ObjectClass Class)
{
	std::vector<Object*> vector = {};
	for (size_t i = 0; i < this->Children->childamount; i++)
	{
		if (ProgramObject* obj = dynamic_cast<ProgramObject*>((*this->Children)[i]))
		{
			if (obj->GetType() == Class)
			{
				vector.push_back(obj);
			}
		}
	}
	return vector;
}

Object** ProgramObject::GetDescendants(size_t* AmountOfDescendants) //WIP
{
	size_t amountofdescendants = 0;

	Object** p = nullptr;

	if (this->Children->childamount > 0)
	{
		//WIP
	}

	if (AmountOfDescendants != nullptr)
	{
		(*AmountOfDescendants) = amountofdescendants;
	}

	return p;
}

void ProgramObject::Release() {
	delete this->Children;
	this->Children = nullptr;
	delete this;
}

void ProgramObject::EnableGradualCleanupWhenDestroyed()
{
	this->Children->EnableGradualCleanup = true;
}

//////////////////////////////////////////////

tn::NormalArray<Sprite*> Workspace::Raycast(tn::Ray2D Ray)
{
	tn::NormalArray<Sprite*> touches = {};
	
	for (size_t i = 0; i < this->Children->Size(); i++)
	{
		if (Object* obj = this->Children->operator[](i))
		{
			if (Sprite* sp = dynamic_cast<Sprite*>(obj))
			{
				tn::Rect rt = sp->GetRect();
				if (tn::AABB_Collision(&Ray, &rt) == true)
				{
					touches.AddToEnd(sp);
				}
			}
		}
	}
	return touches;
}

//////////////////////////////////////////////

Collider2D::Collider2D(Program* m_Program) : ProgramObject(m_Program)
{

}

ObjArray Collider2D::GetIntersectionsWithin(ProgramObject* Obj)
{
	ObjArray oa = {};
	if (BaseObject)
	{
		size_t amountofchildren = 0;

		ObjArray childrenofobj = Obj->GetChildren();
		for (size_t i = 0; i < amountofchildren; i++)
		{
			if (Sprite* s = dynamic_cast<Sprite*>(childrenofobj[i]))
			{
				if (this->IsOverlappedWith(s) == true)
				{
					oa.Add(s);
				}
			}
		}
	}
	return oa;
}

bool Collider2D::IsOverlappedWith(Object* Object)
{
	if (Sprite* bsp = dynamic_cast<Sprite*>(BaseObject))
	{
		if (Sprite* sp = dynamic_cast<Sprite*>(Object))
		{
			tn::Rect rt1, rt2;
			tn::Convert2DVertexIntoRect(bsp->Vertices, &rt1);
			tn::Convert2DVertexIntoRect(sp->Vertices, &rt2);

			tn::TransformRect(&rt1, bsp->Position);
			tn::TransformRect(&rt2, sp->Position);

			return tn::AABB_Collision(rt1, rt2);
		}
	}

	return false;
}

bool Collider2D::IsOverlappedWithUI(UI_Frame* Object)
{
	return false;
}

void Collider2D::OnMouseClick(void (*Function) (Collider2D* Body, MouseObject* Mouse))
{
	this->MouseClickFunc = Function;
}