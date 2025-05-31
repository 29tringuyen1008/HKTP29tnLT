#pragma once
#include <type_traits>

namespace tn
{
	template <typename T>
	class ReferenceArray
	{
	public:
		ReferenceArray()
		{
			Base = new T * [1];
			Base[0] = nullptr;
		}
		ReferenceArray(const ReferenceArray& ArrayToCopy)
		{
			Base = new T * [ArrayToCopy.max];
			max = ArrayToCopy.max;
			size = ArrayToCopy.size;
			for (size_t i = 0; i < ArrayToCopy.max; i++)
			{
				Base[i] = ArrayToCopy.Base[i];
			}
		}
		~ReferenceArray()
		{
			delete[] Base;
			Base = nullptr;
		}

		void Add(T* obj)
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

			size++;
		};

		void Remove(T* obj)
		{
			bool EmptyFound = false;
			size_t index = 0;
			for (size_t i = 0; i < max; ++i) {
				if (Base[i] == obj)
				{
					Base[i] = nullptr;
					size--;
					this->ShrinkToFit();
					break;
				}
			}
		};

		void Swap(T* obj1, T* obj2)
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
					T* obj1 = Base[index1];
					Base[index1] = Base[index2];
					Base[index2] = obj1;
					break;
				};
			}
		}

		void Clear()
		{
			for (size_t i = 0; i < max; ++i) {
				Base[i] = nullptr;
			}
		}

		const T* operator[](size_t i) const {
			if (i < this->max)
			{
				return Base[i];
			}
			else
			{
				return nullptr;
			}
		};

		T* operator[](size_t i) {
			if (i < this->max)
			{
				return Base[i];
			}
			else
			{
				return nullptr;
			}
		};

		const T* begin() const { return this->Base[0]; };
		const T* end() const { return this->Base[this->size]; };

		size_t GetSize() { return this->size; };
	private:
		void Expand()
		{
			T** OldBase = Base;
			size_t oldmax = max;
			max += 1;
			Base = new T * [max];
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
				T** OldBase = Base;
				size_t oldmax = max;
				max -= emptycount;
				Base = new T * [max];

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
		size_t size = 0;
		T** Base;
	};

	template <typename T>
	class NormalArray
	{
	public:
		constexpr NormalArray() noexcept
		{
			Base = nullptr;
			max_size = 0;
		}

		constexpr NormalArray(NormalArray&& other) noexcept {
			Base = other.Base;
			max_size = other.max_size;
			other.Base = nullptr;
			other.max_size = 0;
		}

		constexpr NormalArray(const NormalArray& NAtocopy) noexcept
		{
			Base = new T[NAtocopy.max_size];
			max_size = NAtocopy.max_size;
			for (size_t i = 0; i < max_size; i++)
			{
				Base[i] = NAtocopy.Base[i];
			}
		}

		constexpr NormalArray(size_t size) noexcept
		{
			Base = new T[size];
			max_size = size;
		}

		template <typename... Args, typename = typename std::enable_if<std::conjunction<std::is_convertible<Args, T>...>::value>::type>
		NormalArray(Args&&... args) {
			T temp[] = { args... };

			size_t numberofparams = sizeof...(args);

			this->Base = new T[numberofparams];
			for (size_t i = 0; i < numberofparams; ++i) {
				Base[i] = temp[i];
			}
			this->max_size = numberofparams;
			(void)temp;
		}

		~NormalArray()
		{
			if (Base != nullptr)
			{
				delete[] Base;
				Base = nullptr;
			}
		}

		NormalArray& operator=(const NormalArray& other) noexcept
		{
			if (this != &other) {
				delete[] Base;
				max_size = other.max_size;
				Base = new T[max_size];
				for (size_t i = 0; i < max_size; ++i)
					Base[i] = other.Base[i];
			}
			return *this;
		}

		NormalArray& operator=(NormalArray&& other) noexcept {
			if (this != &other) {
				Base = other.Base;
				max_size = other.max_size;
				other.Base = nullptr;
				other.max_size = 0;
			}
			return *this;
		}

		size_t AddToEnd(T Variable)
		{
			size_t newsize = this->max_size + 1;
			Resize(newsize);
			this->Base[newsize - 1] = Variable;
			return newsize - 1;
		}

		void Resize(size_t _new_size)
		{
			if (_new_size > this->max_size)
			{
				size_t oldsize = this->max_size;
				T* OldBase = Base;
				T* NewBase = new T[_new_size];

				for (size_t i = 0; i < oldsize; ++i)
				{
					NewBase[i] = OldBase[i]; 
				}

				for (size_t i = oldsize; i < _new_size; ++i)
				{
					NewBase[i] = T();
				}

				delete[] OldBase;
				Base = NewBase;
				this->max_size = _new_size;
			}
		}

		T& operator[](size_t index)
		{
			if (max_size > 0)
			{
				if (index >= 0 and index < max_size)
				{
					return Base[index];
				}

				return Base[0];
			}
			else
			{
				T newelement = T();
				return newelement;
			}
		}

		size_t GetMaximumSize() const
		{
			return max_size;
		}

		//Giống như GetMaximumSize()
		size_t length() const { return max_size; };
		//Giống như GetMaximumSize()
		size_t size() const { return max_size; };

		T& begin()
		{
			return this->Base[0];
		}

		T& end()
		{
			return this->Base[max_size-1];
		}

		const T* Data() const noexcept
		{
			return this->Base;
		}

		T* Data() noexcept
		{
			return this->Base;
		}

		//Giống như Data()
		const T* data() const noexcept { return this->Base;}
		//Giống như Data()
		T* data() noexcept { return this->Base; }
	private:
		size_t max_size = 0;
		T* Base = nullptr;
	};
}

template <typename T>
bool operator == (tn::NormalArray<T>& NArray1, tn::NormalArray<T>& NArray2)
{
	if (NArray1.GetMaximumSize() != NArray2.GetMaximumSize())
		return false;
	for (size_t i = 0; i < NArray1.GetMaximumSize(); ++i) {
		if (NArray1[i] != NArray2[i])
			return false;
	}
	return true;
};

template <typename T>
bool operator != (tn::NormalArray<T>& NArray1, tn::NormalArray<T>& NArray2)
{
	if (NArray1.GetMaximumSize() != NArray2.GetMaximumSize())
		return true;
	for (size_t i = 0; i < NArray1.GetMaximumSize(); ++i) {
		if (NArray1[i] == NArray2[i])
			return false;
	}
	return true;
};