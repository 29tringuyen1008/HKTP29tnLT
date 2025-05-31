#include "String.h"
#include <Windows.h>
#include <cassert>

bool operator==(const tn::String& str1, const tn::String& str2)
{
	if (str1.length == str2.length)
	{
		size_t same_length = str1.length;

		for (size_t i = 0; i < same_length; i++)
		{
			if (str1.Base[i] != str2.Base[i])
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	return true;
}

bool operator!=(const tn::String& str1, const tn::String& str2)
{
	return not (str1 == str2);
}

bool operator==(const tn::String& str1, const char* str2)
{
	if (str1.Base == nullptr) return false;
	size_t raw_len = strlen(str2);
	if (str1.length == raw_len)
	{
		size_t same_length = str1.length;

		for (size_t i = 0; i < same_length; i++)
		{
			if (static_cast<int>(str1.Base[i]) != static_cast<int>(str2[i]))
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	return true;
}

bool operator!=(const tn::String& str1, const char* str2)
{
	return not (str1 == str2);
}

bool operator==(const tn::String& str1, const wchar_t* str2)
{
	if (str1.Base == nullptr) return false;
	size_t raw_len = wcslen(str2);
	if (str1.length == raw_len)
	{
		size_t same_length = str1.length;

		for (size_t i = 0; i < same_length; i++)
		{
			if (static_cast<int>(str1.Base[i]) != static_cast<int>(str2[i]))
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	return true;
}

bool operator!=(const tn::String& str1, const wchar_t* str2)
{
	return not (str1 == str2);
}

tn::String operator+(const tn::String& primary_str, const tn::String& additional_str)
{
	tn::String newStr = primary_str;

	if (additional_str.length > 0)
	{
		wchar_t* oldBase = newStr.Base;

		size_t oldLength = newStr.length;
		newStr.length += additional_str.length;

		wchar_t* temp = (wchar_t*)realloc(oldBase, sizeof(wchar_t) * (newStr.length + 1));
		if (!temp) {
			return oldBase;
		}
		newStr.Base = temp;

		newStr.Base[oldLength] = L'\0';

		wcscat_s(newStr.Base, newStr.length + 1, additional_str.Base);
		newStr.Base[newStr.length] = L'\0';
	}

	return newStr;
}

tn::String operator+(const tn::String& primary_str, const char* additional_str)
{
	tn::String newStr = primary_str;
	if (additional_str != nullptr)
	{
		size_t raw_len = strlen(additional_str);
		if (raw_len > 0)
		{
			wchar_t* oldBase = newStr.Base;

			size_t oldlength = newStr.length;
			newStr.length += raw_len;

			newStr.Base = (wchar_t*)malloc(sizeof(wchar_t) * (newStr.length + 1));

			if (oldBase != nullptr)
			{
				for (size_t i = 0; i < oldlength; i++)
				{
					newStr.Base[i] = oldBase[i];
				}
				free(oldBase);
			}

			for (size_t i = oldlength; i < newStr.length; i++)
			{
				newStr.Base[i] = static_cast<wchar_t>(additional_str[i]);
			}
			newStr.Base[newStr.length] = L'\0';
		}
	}
	return newStr;
}

tn::String operator+(const tn::String& primary_str, const wchar_t* additional_str)
{
	tn::String newStr = primary_str;

	if (additional_str != nullptr)
	{
		size_t raw_len = wcslen(additional_str);
		if (raw_len > 0)
		{
			wchar_t* oldBase = newStr.Base;

			size_t oldLength = newStr.length;
			newStr.length += raw_len;

			wchar_t* temp = (wchar_t*)realloc(oldBase, sizeof(wchar_t) * (newStr.length + 1));
			if (!temp) {
				return oldBase;
			}
			newStr.Base = temp;

			newStr.Base[oldLength] = L'\0';

			wcscat_s(newStr.Base, newStr.length + 1, additional_str);
			newStr.Base[newStr.length] = L'\0';
		}
	}

	return newStr;
}

/////////////////////////////////////////////////////////////////////////////////

tn::String::String() : Base(nullptr), length(0)
{
	this->Base = nullptr;
	this->length = 0;
}

tn::String::String(tn::String&& otherstring) noexcept
{
	this->Base = otherstring.Base;
	this->length = otherstring.length;

	otherstring.Base = nullptr;
	otherstring.length = 0;
}

tn::String::String(const tn::String& oldCopy)
{
	this->Base = nullptr;

	this->length = oldCopy.length;
	this->Base = (wchar_t*)malloc(sizeof(wchar_t)*this->length);
	for (size_t i = 0; i < this->length; i++)
	{
		this->Base[i] = oldCopy.Base[i];
	}
}

tn::String::String(const char* raw_string, Encodings Encoding)
{
	this->Base = nullptr;

	int def_encoding = CP_UTF8;
	if (Encoding == Encodings::UTF_7)
	{
		def_encoding = CP_UTF7;
	}
	else if (Encoding == Encodings::UTF_8)
	{
		def_encoding = CP_UTF8;
	}
	else if (Encoding == Encodings::ACP)
	{
		def_encoding = CP_ACP;
	}
	else if (Encoding == Encodings::OEMCP)
	{
		def_encoding = CP_OEMCP;
	}

	size_t mb_len = strlen(raw_string);
	size_t numberofcharconverted = MultiByteToWideChar(def_encoding, 0, raw_string, static_cast<int>(mb_len), nullptr, 0);
	if (numberofcharconverted > 0)
	{
		this->length = numberofcharconverted;
		this->Base = (wchar_t*)malloc(sizeof(wchar_t) * this->length);
		MultiByteToWideChar(def_encoding, 0, raw_string, static_cast<int>(mb_len), this->Base, numberofcharconverted);
	}
	else
	{
		this->length = 0;
	}
}

tn::String::String(const char* raw_string)
{
	EnterCriticalSection();
	size_t raw_str_len = strlen(raw_string);
	length = raw_str_len;
	this->Base = nullptr;
	this->Base = (wchar_t*)malloc(sizeof(wchar_t) * this->length);
	for (size_t i = 0; i < length; i++)
	{
		this->Base[i] = (wchar_t)raw_string[i];
	}
	ExitCriticalSection();
}

tn::String::String(const wchar_t* raw_string)
{
	EnterCriticalSection();
	size_t raw_str_len = wcslen(raw_string);
	length = raw_str_len;
	this->Base = nullptr;
	this->Base = (wchar_t*)malloc(sizeof(wchar_t) * this->length);
	for (size_t i = 0; i < length; i++)
	{
		this->Base[i] = raw_string[i];
	}
	ExitCriticalSection();
}

tn::String::String(std::wstring std_wstring)
{
	length = std_wstring.length();
	this->Base = nullptr;
	this->Base = (wchar_t*)malloc(sizeof(wchar_t) * this->length);
	for (size_t i = 0; i < length; i++)
	{
		this->Base[i] = std_wstring.at(i);
	}
}

tn::String& tn::String::operator=(const tn::String& other)
{
	if (this != &other) {
		if (other.length > 0)
		{
			wchar_t* OldBase = this->Base;
			this->length = other.length;
			wchar_t* NewBase = (wchar_t*)malloc(sizeof(wchar_t) * this->length);
			for (size_t i = 0; i < this->length; i++)
			{
				NewBase[i] = other.Base[i];
			}
			delete[] OldBase;
			this->Base = NewBase;
		}
	}
	return *this;
}

tn::String& tn::String::operator=(const char* strchar)
{
	wchar_t* oldBase = nullptr;
	if (Base) oldBase = Base;

	size_t len = strlen(strchar);
	this->length = len;
	Base = (wchar_t*)malloc(sizeof(wchar_t) * this->length);
	for (size_t i = 0; i < this->length; ++i)
		Base[i] = static_cast<wchar_t>(strchar[i]);

	if (oldBase) free(oldBase);
	return *this;
}

tn::String& tn::String::operator=(const wchar_t* strwchar)
{
	wchar_t* oldBase = nullptr;
	if (Base) oldBase = Base;

	size_t len = wcslen(strwchar);
	this->length = len;
	Base = (wchar_t*)malloc(sizeof(wchar_t) * this->length);
	for (size_t i = 0; i < this->length; ++i)
		Base[i] = strwchar[i];

	if (oldBase) free(oldBase);
	return *this;
}

tn::String::operator std::wstring()
{
	std::wstring newstdwstring = L"\0";
	newstdwstring.resize(this->length);
	for (size_t i = 0; i < this->length; i++)
	{
		newstdwstring.at(i) = this->Base[i];
	}
	return newstdwstring;
}

tn::String::~String()
{
	if (this->Base != nullptr)
	{
		delete[] this->Base;
		this->Base = nullptr;
	}
}

wchar_t& tn::String::At(size_t index) const
{
	return this->Base[index];
}

size_t tn::String::GetLength()
{
	return this->length;
}

wchar_t* tn::String::CopyTo()
{
	if (length > 0)
	{
		wchar_t* OppStr = new wchar_t[length];
		for (size_t i = 0; i < length; i++)
		{
			OppStr[i] = this->Base[i];
		}
		return OppStr;
	}
	else
	{
		return nullptr;
	}
}

wchar_t*& tn::String::Data()
{
	return this->Base;
}

char* tn::String::CopyAsMultiByte(Encodings Encoding)
{
	if (length > 0)
	{
		int def_encoding = CP_UTF8;
		if (Encoding == Encodings::UTF_7)
		{
			def_encoding = CP_UTF7;
		}
		else if (Encoding == Encodings::UTF_8)
		{
			def_encoding = CP_UTF8;
		}
		else if (Encoding == Encodings::ACP)
		{
			def_encoding = CP_ACP;
		}
		else if (Encoding == Encodings::OEMCP)
		{
			def_encoding = CP_OEMCP;
		}

		char* NewStr = nullptr;
		size_t numberofcharconverted = WideCharToMultiByte(def_encoding, 0, this->Base, length, nullptr, 0, nullptr, nullptr);
		if (numberofcharconverted > 0)
		{
			NewStr = new char[numberofcharconverted];
			WideCharToMultiByte(CP_UTF8, 0, this->Base, length, NewStr, numberofcharconverted, nullptr, nullptr);
		}
		return NewStr;
	}
	else
	{
		return nullptr;
	}
}

size_t tn::String::Resize(size_t _newsize)
{
	if (this->length < _newsize)
	{
		size_t oldSize = this->length;
		size_t newSize = _newsize;

		this->Base = (wchar_t*)realloc(this->Base, sizeof(wchar_t) * newSize);
		this->length = newSize;
	}
	else
	{
		size_t oldSize = this->length;
		size_t newSize = _newsize;
		wchar_t* PreviousBase = this->Base;
		this->Base = nullptr;

		memcpy_s(this->Base, sizeof(wchar_t) * newSize, PreviousBase, sizeof(wchar_t) * this->length);
		free(PreviousBase);
		this->length = newSize;
	}
	return this->length;
}

void tn::String::EnterCriticalSection()
{
	if (this->InCriticalSection == true)
	{
		while (this->InCriticalSection == true) Sleep(1);
	}
	
	this->InCriticalSection = true;
}

void tn::String::ExitCriticalSection()
{
	this->InCriticalSection = false;
}