#pragma once
//////////TỆP NÀY KHÔNG ĐƯỢC DÙNG (UNUSED)
#include <string> //std::string

namespace tn { class String; };
bool operator==(const tn::String& str1, const tn::String& str2);
bool operator!=(const tn::String& str1, const tn::String& str2);
bool operator==(const tn::String& str1, const char* str2);
bool operator!=(const tn::String& str1, const char* str2);
bool operator==(const tn::String& str1, const wchar_t* str2);
bool operator!=(const tn::String& str1, const wchar_t* str2);
tn::String operator+(const tn::String& primary_str, const tn::String& additional_str);
tn::String operator+(const tn::String& primary_str, const char* additional_str);
tn::String operator+(const tn::String& primary_str, const wchar_t* additional_str);

namespace tn
{
	enum Encodings
	{
		UTF_7 = 0,
		UTF_8 = 1,
		ACP = 2,
		OEMCP = 3,
	};

	class String
	{
	private:
		size_t length = 0;
		void* HeapBuffer = nullptr;
		wchar_t* Base = nullptr;
		bool InCriticalSection = false;

		friend bool ::operator==(const tn::String& str1, const tn::String& str2);
		friend bool ::operator!=(const tn::String& str1, const tn::String& str2);
		friend bool ::operator==(const tn::String& str1, const char* str2);
		friend bool ::operator!=(const tn::String& str1, const char* str2);
		friend bool ::operator==(const tn::String& str1, const wchar_t* str2);
		friend bool ::operator!=(const tn::String& str1, const wchar_t* str2);
		friend auto ::operator+(const tn::String& primary_str, const tn::String& additional_str) -> tn::String;
		friend auto ::operator+(const tn::String& primary_str, const char* additional_str) -> tn::String;
		friend auto ::operator+(const tn::String& primary_str, const wchar_t* additional_str) -> tn::String;
		
		void EnterCriticalSection();
		void ExitCriticalSection();
	public:
		String();
		String(String&& otherstring) noexcept;
		String(const String& oldCopy);
		String(const char* raw_string);
		String(const char* raw_string, Encodings Encoding);
		String(const wchar_t* raw_string);
		String(std::wstring std_wstring);

		String& operator=(const String& other);
		String& operator=(const char* strchar);
		String& operator=(const wchar_t* strwchar);

		operator std::wstring();

		~String();

		wchar_t& At(size_t index) const;
		wchar_t* CopyTo();
		wchar_t*& Data();
		char* CopyAsMultiByte(Encodings Encoding);
		size_t Resize(size_t newSize);

		size_t GetLength();
	};
};