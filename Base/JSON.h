#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <Windows.h>
#include <iomanip>

enum class JSONElementTypeEnum
{
	Null = 0,
	String = 1,
	Number = 2,
	Boolean = 3,
	Array = 4,
	Object = 5
};

class JSONParser;
class JSONGenerator;

class JSONElement
{
private:
	friend class JSONParser;
	friend class JSONGenerator;

	std::wstring ElementName = L"";
	std::wstring String = L"";
	float Number = 0;
	bool Boolean = false;
	bool Null = false;
	std::vector<JSONElement>ChildElements;
	JSONElementTypeEnum EnumType;
public:
	JSONElement() {
		this->Null = true;
	};
	JSONElement(const JSONElement& js) {
		this->Null = js.Null;
		this->Boolean = js.Boolean;
		this->Number = js.Number;
		this->String = js.String;
		this->ElementName = js.ElementName;
		this->ChildElements = js.ChildElements;
		this->EnumType = js.EnumType;
	};
	~JSONElement() {
		ClearAllChildElements();
	};
	JSONElement(int NumberValue) {
		this->Number = NumberValue;
		this->EnumType = JSONElementTypeEnum::Number;
	};
	JSONElement(double NumberValue) {
		this->Number = NumberValue;
		this->EnumType = JSONElementTypeEnum::Number;
	};
	JSONElement(float NumberValue) {
		this->Number = NumberValue;
		this->EnumType = JSONElementTypeEnum::Number;
	};
	JSONElement(bool BooleanValue) {
		this->Boolean = BooleanValue;
		this->EnumType = JSONElementTypeEnum::Boolean;
	};
	JSONElement(char* StringValue) {
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, StringValue, -1, NULL, 0);
		wchar_t* wCharArray = new wchar_t[size_needed];
		MultiByteToWideChar(CP_UTF8, 0, StringValue, -1, wCharArray, size_needed);

		this->String = wCharArray;
		this->EnumType = JSONElementTypeEnum::String;
	};
	JSONElement(const char* StringValue) {
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, StringValue, -1, NULL, 0);
		wchar_t* wCharArray = new wchar_t[size_needed];
		MultiByteToWideChar(CP_UTF8, 0, StringValue, -1, wCharArray, size_needed);

		this->String = wCharArray;
		this->EnumType = JSONElementTypeEnum::String;
	};
	JSONElement(wchar_t* StringValue) {
		this->String = StringValue;
		this->EnumType = JSONElementTypeEnum::String;
	};
	JSONElement(const wchar_t* StringValue) {
		this->String = StringValue;
		this->EnumType = JSONElementTypeEnum::String;
	};
	JSONElement(std::wstring StringValue) {
		this->String = StringValue.c_str();
		this->EnumType = JSONElementTypeEnum::String;
	};
	JSONElement(std::nullptr_t null) {
		this->Null = true;
		this->EnumType = JSONElementTypeEnum::Null;
	};

	template <typename T, typename... Args>
	JSONElement(T firstelement, Args... elements) {
		this->InsertElementArray(firstelement, elements...);
		this->EnumType = JSONElementTypeEnum::Array;
	};

	void SetEnumType(JSONElementTypeEnum te)
	{
		this->EnumType = te;
	}
	JSONElementTypeEnum GetEnumType()
	{
		return this->EnumType;
	}

	void SetName(const wchar_t* Name)
	{
		this->ElementName = Name;
	}
	void InitializeAsObject()
	{
		this->EnumType = JSONElementTypeEnum::Object;
	}
	void RemoveChildElement(const wchar_t* ElementName)
	{
		for (ULONG_PTR i = 0; i < ChildElements.size(); i++)
		{
			if (ChildElements[i].ElementName == ElementName)
			{
				ChildElements.erase(ChildElements.begin() + i);
				ChildElements.shrink_to_fit();
				break;
			}
		}
	}
	void RemoveChildElements(const wchar_t* WithElementName)
	{
		for (ULONG_PTR i = 0; i < ChildElements.size(); i++)
		{
			if (ChildElements[i].ElementName == WithElementName)
			{
				ChildElements.erase(ChildElements.begin() + i);
			}
		}
		ChildElements.shrink_to_fit();
	}
	void ClearAllChildElements()
	{
		ChildElements.clear();
	}
	const std::vector<JSONElement>& GetChildElements() const
	{
		return ChildElements;
	}
	size_t GetNumberOfChildElements()
	{
		return ChildElements.size();
	}
	void RemoveChildElement(int ElementIndex)
	{
		if (ElementIndex < ChildElements.size())
		{
			ChildElements.erase(ChildElements.begin() + ElementIndex);
			ChildElements.shrink_to_fit();
		}
	}

	JSONElement& operator[](const char* elementname)
	{
		for (ULONG_PTR i = 0; i < ChildElements.size(); i++)
		{
			int size_needed = MultiByteToWideChar(CP_UTF8, 0, elementname, -1, NULL, 0);
			wchar_t* wCharArray = new wchar_t[size_needed];
			MultiByteToWideChar(CP_UTF8, 0, elementname, -1, wCharArray, size_needed);

			if (ChildElements.at(i).ElementName == wCharArray)
			{
				return ChildElements.at(i);
			}

			delete[] wCharArray;
		}
		JSONElement js = nullptr;
		return js;
	}

	JSONElement& operator[](const wchar_t* elementname)
	{
		for (ULONG_PTR i = 0; i < ChildElements.size(); i++)
		{
			if (ChildElements.at(i).ElementName == elementname)
			{
				return ChildElements.at(i);
			}
		}
		JSONElement js = nullptr;
		return js;
	}

	JSONElement& operator[](size_t elementindex)
	{
		if (elementindex < ChildElements.size())
		{
			return ChildElements.at(elementindex);
		}
		JSONElement js = nullptr;
		return js;
	}

	operator int()
	{
		return Number;
	}
	operator float()
	{
		return Number;
	}
	operator double()
	{
		return (double)Number;
	}
	operator long double()
	{
		return (long double)Number;
	}

	operator bool()
	{
		return Boolean;
	}

	operator std::wstring()
	{
		return String;
	}

	operator const wchar_t* ()
	{
		return String.c_str();
	}

	void operator<<(JSONElement element)
	{
		if (this->EnumType == JSONElementTypeEnum::Array or this->EnumType == JSONElementTypeEnum::Object)
		{
			ChildElements.push_back(element);
		}
	}
private:
	void InsertElementArray() {};

	template <typename InitialData>
	void InsertElementArray(InitialData firstelement)
	{
		JSONElement js = firstelement;
		ChildElements.push_back(js);
		this->EnumType = JSONElementTypeEnum::Array;
	}

	template <typename InitialData, typename... Args>
	void InsertElementArray(InitialData firstelement, Args... otherelements)
	{
		JSONElement js = firstelement;
		ChildElements.push_back(js);
		this->InsertElementArray(otherelements...);
		this->EnumType = JSONElementTypeEnum::Array;
	}
};

class JSONParser
{
private:
	HANDLE hFile;
	JSONElement* Output;
	DWORD bytestoallocatepertask = 2048;
public:
	JSONParser()
	{
		hFile = nullptr;
	}
	~JSONParser()
	{
		if (charbuffer)
		{
			delete[] charbuffer;
		}
		if (currentbuffer)
		{
			delete[] currentbuffer;
		}
	}

	void LoadFromFile(HANDLE hFileTarget)
	{
		hFile = hFileTarget;
	}

	void SetOutput(JSONElement* o)
	{
		this->Output = o;
	}

	int StartParse()
	{
		FileSize = GetFileSize(hFile, nullptr);
		OVERLAPPED overlapped = { 0 };

		charbuffer = new char[FileSize];
		BOOL success = ReadFile(hFile, static_cast<LPVOID>(charbuffer), FileSize, nullptr, nullptr);
		if (success == FALSE)
		{
			return GetLastError();
		}

		int wideSize = MultiByteToWideChar(CP_UTF8, 0, charbuffer, -1, nullptr, 0);
		currentbuffer = new wchar_t[wideSize];
		MultiByteToWideChar(CP_UTF8, 0, charbuffer, -1, currentbuffer, wideSize);

		delete[] charbuffer;
		charbuffer = nullptr;
		buffercount++;

		//NextRead();
		SkipWhitespace();
		(*this->Output) = ParseValue();

		return 0;
	}
private:
	DWORD allocatednumberofbytes;
	DWORD index = 0;
	ULONG_PTR FileSize;
	ULONG_PTR GlobalIndex = 0;
	bool Finished = false;
	int heapcount; ULONG_PTR buffercount = 0;
	wchar_t* currentbuffer = nullptr;
	char* charbuffer = nullptr;

	bool IsFinished() { return Finished; };

	void NextIndex()
	{
		if (Finished == false)
		{
			/*
			if (index >= allocatednumberofbytes) {
				BOOL success = ReadFile(hFile, static_cast<LPVOID>(buffer), bytestoallocatepertask, &allocatednumberofbytes, nullptr);
				if (success == FALSE) return;

				int wideSize = MultiByteToWideChar(CP_UTF8, 0, buffer, -1, nullptr, 0);
				if ((buffercount + 1) % 2 != 0)
				{
					wideBuffer = new wchar_t[wideSize];
					MultiByteToWideChar(CP_UTF8, 0, buffer, -1, wideBuffer, wideSize);
					currentbuffer = wideBuffer;
				}
				else
				{
					wideBuffer2 = new wchar_t[wideSize];
					MultiByteToWideChar(CP_UTF8, 0, buffer, -1, wideBuffer, wideSize);
					currentbuffer = wideBuffer2;
				}

				//std::cout << "The parser has read " << allocatednumberofbytes << " bytes" << std::endl;
				buffercount++;
			}
			*/
			index++;

			if (GlobalIndex >= FileSize) {
				Finished = true;
				return;
			}
			GlobalIndex++;
		}
	}

	/*
	int NextRead()
	{
		BOOL success = ReadFile(hFile, static_cast<LPVOID>(buffer), bytestoallocatepertask, &allocatednumberofbytes, nullptr);
		if (success == FALSE) return -1;

		int wideSize = MultiByteToWideChar(CP_UTF8, 0, buffer, -1, nullptr, 0);
		wideBuffer = new wchar_t[wideSize];
		MultiByteToWideChar(CP_UTF8, 0, buffer, -1, wideBuffer, wideSize);

		std::cout << "The parser has read " << allocatednumberofbytes << " bytes" << std::endl;
		std::wcout << L"Text: " << wideBuffer << std::endl;
		index = 0;

		if (bytestoallocatepertask > allocatednumberofbytes) Finished = true;
		return 0;
	}
	*/

	void SkipUntil(wchar_t c)
	{
		while (currentbuffer[index] != c and this->Finished == false)
		{
			NextIndex();
		}
	}

	void SkipWhitespace()
	{
		while (iswspace(currentbuffer[index]) and this->Finished == false)
		{
			NextIndex();
		}
		if (currentbuffer[index] == L'/' and (currentbuffer[index + 1] == L'/' or currentbuffer[index + 1] == L'*'))
		{
			SkipComments();
			SkipWhitespace();
		}
	}

	bool Expect(wchar_t c)
	{
		if (currentbuffer[index] == c) return true;
		else
		{
			//std::wcout << L"Expected: '" << c << L"' but found '" << currentbuffer[index] << L"' with the char number is " << (int)currentbuffer[index] << L" at " << index << std::endl;
			return false;
		}
	}

	bool CheckIf(wchar_t c)
	{
		if (currentbuffer[index] == c)
		{
			NextIndex();
			return true;
		}
		else return false;
	}

	void SkipComments() {
		//std::wcout << currentbuffer[index] << std::endl;
		if (currentbuffer[index] == L'/' && currentbuffer[index + 1] == L'/') {
			NextIndex(); NextIndex();
			while (currentbuffer[index] != L'\n' && !Finished) {
				NextIndex();
			}

			if (currentbuffer[index] == L'\n') {
				NextIndex();
			}
		}
		else if (currentbuffer[index] == L'/' && currentbuffer[index + 1] == L'*') {
			NextIndex(); NextIndex();
			while ((currentbuffer[index] != L'*' and currentbuffer[index + 1] != L'/') && !Finished) {
				NextIndex();
			}

			if (currentbuffer[index] == L'*' and currentbuffer[index + 1] == L'/') {
				NextIndex(); NextIndex();
			}
		}
	}

	JSONElement ParseValue()
	{
		JSONElement js = {};

		/*
		SkipWhitespace();
		if (!Expect('"')) return false;
		char* name = ParseString();
		SkipWhitespace();
		if (!Expect(':')) return false;
		index++;
		SkipWhitespace();
		*/
		SkipWhitespace();
		//std::cout << "key " << textread[index] << " " << int(textread[index]) << std::endl;
		if (currentbuffer[index] == L'/')
		{
			SkipComments();
		}
		else if (currentbuffer[index] == L'"')
		{
			js = ParseString();
			js.EnumType = JSONElementTypeEnum::String;
		}
		else if (currentbuffer[index] == L'-' or (currentbuffer[index] >= L'0' and currentbuffer[index] <= L'9'))
		{
			js = ParseNumber();
			js.EnumType = JSONElementTypeEnum::Number;
			//index++;
		}
		else if (currentbuffer[index] == L'[')
		{
			js = ParseArray();
			js.EnumType = JSONElementTypeEnum::Array;
		}
		else if (currentbuffer[index] == L'{')
		{
			js = ParseObject();
			js.EnumType = JSONElementTypeEnum::Object;
			//index++;
		}
		else if (currentbuffer[index] == L't' or currentbuffer[index] == L'f')
		{
			js = ParseBoolean();
			js.EnumType = JSONElementTypeEnum::Boolean;
			//index++;
		}
		else if (currentbuffer[index] == L'n')
		{
			js = ParseNull();
			js.EnumType = JSONElementTypeEnum::Null;
			//index++;
		}
		else if (currentbuffer[index] == L'\0')
		{
			//std::cout << "Unexpected end of file while parsing string." << std::endl;
			return js;
		}
		else
		{
			//std::cout << "Invalid key at " << index << "!" << std::endl;
			return js;
		}

		return js;
	}

	std::wstring ParseString()
	{
		std::wstring text;
		NextIndex();

		while (currentbuffer[index] != L'"' && currentbuffer[index] != L'\0') {
			text += currentbuffer[index++];
		}

		NextIndex();
		return text;
	}

	float ParseNumber()
	{
		float number = 0.0f;
		float negativemode = 1.0f;
		float multiply = 1.0f;

		if (currentbuffer[index] == L'-')
		{
			negativemode = -1.0f;
			NextIndex();
		}

		bool hasDigits = false;
		while (std::isdigit(currentbuffer[index]))
		{
			hasDigits = true;
			number = number * 10 + (currentbuffer[index] - L'0');
			NextIndex();
		}

		if (!hasDigits) return 0.0f;

		if (currentbuffer[index] == L'.')
		{
			NextIndex();
			float fractionalValue = 0.0f;
			float decimalPlace = 10.0f;

			while (std::isdigit(currentbuffer[index]))
			{
				fractionalValue += (currentbuffer[index] - L'0') / decimalPlace;
				decimalPlace *= 10;
				NextIndex();
			}
			number += fractionalValue;
		}

		return number * negativemode;
	}

	bool ParseBoolean()
	{
		if (currentbuffer[index] == L't')
		{
			NextIndex();
			if (currentbuffer[index] == L'r') NextIndex();
			else
			{
				std::cout << "Invalid boolean value";
				SkipUntil(L',');
				return false;
			}

			if (currentbuffer[index] == L'u') NextIndex();
			else
			{
				std::cout << "Invalid boolean value";
				SkipUntil(L',');
				return false;
			}

			if (currentbuffer[index] == L'e')
			{
				NextIndex();
				return true;
			}
			else
			{
				std::cout << "Invalid boolean value";
				SkipUntil(L',');
				return false;
			}
		}
		else if (currentbuffer[index] == L'f')
		{
			NextIndex();
			if (currentbuffer[index] == L'a') NextIndex();
			else
			{
				std::cout << "Invalid boolean value";
				SkipUntil(L',');
				return false;
			}

			if (currentbuffer[index] == L'l') NextIndex();
			else
			{
				std::cout << "Invalid boolean value";
				SkipUntil(L',');
				return false;
			}

			if (currentbuffer[index] == L's') NextIndex();
			else
			{
				std::cout << "Invalid boolean value";
				SkipUntil(L',');
				return false;
			}

			if (currentbuffer[index] == L'e')
			{
				NextIndex();
				return false;
			}
			else
			{
				std::cout << "Invalid boolean value";
				SkipUntil(L',');
				return false;
			}
		}

		return false;
	}

	JSONElement ParseNull()
	{
		NextIndex();

		if (currentbuffer[index] == L'u') NextIndex();
		else
		{
			std::cout << "Invalid boolean value";
			SkipUntil(L',');
			return 0;
		}

		if (currentbuffer[index] == L'l') NextIndex();
		else
		{
			std::cout << "Invalid boolean value";
			SkipUntil(L',');
			return 0;
		}

		if (currentbuffer[index] == L'l')
		{
			NextIndex();
			return JSONElement();
		}
		else
		{
			std::cout << "Invalid boolean value";
			SkipUntil(L',');
			return 0;
		}

		return 0;
	}

	JSONElement ParseObject()
	{
		NextIndex();
		SkipWhitespace();

		JSONElement obj;
		obj.InitializeAsObject();

		if (currentbuffer[index] != L'}') {
			do {
				SkipWhitespace();

				if (!Expect(L'"')) {
					std::wcerr << L"Expected '\"' at start of object key\n";
					return obj;
				}

				std::wstring name = ParseString();
				SkipWhitespace();

				if (!Expect(L':')) {
					std::wcerr << L"Expected ':' after key \"" << name << L"\"\n";
					return obj;
				}
				NextIndex();
				SkipWhitespace();

				JSONElement value = ParseValue();
				value.SetName(name.c_str());
				obj.ChildElements.push_back(value);

				SkipWhitespace();
			} while (CheckIf(L','));
		}

		if (!Expect(L'}')) {
			std::wcerr << L"Expected closing '}' for object\n";
		}
		NextIndex();
		return obj;
	}

	JSONElement ParseArray()
	{
		NextIndex();
		SkipWhitespace();

		JSONElement obj;
		obj.SetEnumType(JSONElementTypeEnum::Array);

		if (currentbuffer[index] != L'}') {
			do {
				SkipWhitespace();

				JSONElement value = ParseValue();
				obj.ChildElements.push_back(value);

				SkipWhitespace();
			} while (CheckIf(L','));
		}

		if (!Expect(L']')) {
			std::wcerr << L"Expected closing ']' for array\n";
		}
		NextIndex();
		return obj;
	}
};

class JSONGenerator
{
private:
	HANDLE hFile = nullptr;
	JSONElement* pElement;
	const wchar_t* Filename;
public:
	JSONGenerator()
	{

	};

	void SetElement(JSONElement* startpElement)
	{
		this->pElement = startpElement;
	}

	void SetFile(const wchar_t* file)
	{
		Filename = file;
	}

	void Start()
	{
		this->hFile = CreateFileW(Filename, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		DrawKeywordsFromElement(pElement);
		CloseHandle(this->hFile);
	}
private:
	DWORD numberofbyteswritten;

	void WriteToFile(const std::wstring& data)
	{
		DWORD numberofbyteswritten;
		int requiredSize = WideCharToMultiByte(CP_UTF8, 0, data.c_str(), -1, nullptr, 0, nullptr, nullptr);
		if (requiredSize > 0)
		{
			std::vector<char> buffer(requiredSize);
			WideCharToMultiByte(CP_UTF8, 0, data.c_str(), -1, buffer.data(), requiredSize, nullptr, nullptr);
			WriteFile(hFile, buffer.data(), requiredSize - 1, &numberofbyteswritten, nullptr); // Exclude the null terminator
		}
	}

	void DrawKeywordsFromElement(JSONElement* jsongen)
	{
		if (jsongen->EnumType == JSONElementTypeEnum::Number)
		{
			std::wostringstream wss;
			wss << std::fixed << std::setprecision(6) << jsongen->Number;
			WriteToFile(wss.str());
		}
		else if (jsongen->EnumType == JSONElementTypeEnum::String)
		{
			const char* quotationmark = "\"";
			WriteFile(this->hFile, quotationmark, 1, &numberofbyteswritten, nullptr);
			WriteToFile(jsongen->String);
			WriteFile(this->hFile, quotationmark, 1, &numberofbyteswritten, nullptr);
		}
		else if (jsongen->EnumType == JSONElementTypeEnum::Boolean)
		{
			if (jsongen->Boolean == true)
			{
				const char* string = "true";
				WriteFile(this->hFile, string, strlen(string), &numberofbyteswritten, nullptr);
			}
			else
			{
				const char* string = "false";
				WriteFile(this->hFile, string, strlen(string), &numberofbyteswritten, nullptr);
			}
		}
		else if (jsongen->EnumType == JSONElementTypeEnum::Array)
		{
			const char* openbracket = "[";
			WriteFile(this->hFile, openbracket, 1, &numberofbyteswritten, nullptr);

			for (size_t i = 0; i < jsongen->ChildElements.size(); i++)
			{
				DrawKeywordsFromElement(&jsongen->ChildElements.at(i));

				if (i < jsongen->ChildElements.size() - 1)
				{
					const char* comma = ",";
					WriteFile(this->hFile, comma, 1, &numberofbyteswritten, nullptr);
				}
			}

			const char* closebracket = "]";
			WriteFile(this->hFile, closebracket, 1, &numberofbyteswritten, nullptr);
		}
		else if (jsongen->EnumType == JSONElementTypeEnum::Object)
		{
			const char* openbracket = "{";
			WriteFile(this->hFile, openbracket, 1, &numberofbyteswritten, nullptr);

			for (size_t i = 0; i < jsongen->ChildElements.size(); i++)
			{
				const char* quotationmark = "\"";
				const char* colon = ":";

				WriteFile(this->hFile, quotationmark, 1, &numberofbyteswritten, nullptr);
				WriteToFile(jsongen->ChildElements.at(i).ElementName);

				WriteFile(this->hFile, quotationmark, 1, &numberofbyteswritten, nullptr);
				WriteFile(this->hFile, colon, 1, &numberofbyteswritten, nullptr);

				DrawKeywordsFromElement(&jsongen->ChildElements.at(i));

				if (i < jsongen->ChildElements.size() - 1)
				{
					const char* comma = ",";
					WriteFile(this->hFile, comma, 1, &numberofbyteswritten, nullptr);
				}
			}

			const char* closebracket = "}";
			WriteFile(this->hFile, closebracket, 1, &numberofbyteswritten, nullptr);
		}
		else
		{
			const char* string = "null";
			WriteFile(this->hFile, string, strlen(string), &numberofbyteswritten, nullptr);
		}
	}
};