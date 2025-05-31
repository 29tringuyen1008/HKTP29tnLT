#pragma once

#include "Memory.h"

#include <iostream>
#include <random>

#define repeat do
#define until(a) while (!(a))

namespace tn
{
	namespace math
	{
		template <typename NumberType>
		inline NumberType absolute(NumberType number)
		{
			NumberType result = number;
			if (result < 0) {
				result = result * (-1);
			}
			return result;
		}

		template <typename NumberType>
		inline NumberType ln(NumberType value)
		{
			NumberType z = (value - 1) / (value + 1);
			NumberType sum = 0;
			for (int n = 1; n <= 500; n += 2) {
				sum += pow(z, n) / n;
			}
			return 2 * sum;
		}

		template <typename NumberType>
		inline NumberType log(NumberType base, NumberType value)
		{
			return ln<NumberType>(value) / ln<NumberType>(base);
		}

		template <typename NumberType>
		inline NumberType clamp(NumberType number, NumberType min, NumberType max)
		{
			if (number < min) {
				return min; // If the value is less than the minimum, return the minimum
			}
			else if (number > max) {
				return max; // If the value is greater than the maximum, return the maximum
			}
			else {
				return number; // Otherwise, return the value itself
			}
			return number;
		}

		template <typename IntegerType>
		inline IntegerType randomInteger(IntegerType min, IntegerType max)
		{
			IntegerType result = 0;

			constexpr bool conditions[] = {
				std::is_same<IntegerType, int>::value == true,
				std::is_same<IntegerType, short>::value == true,
				std::is_same<IntegerType, long>::value == true,
				std::is_same<IntegerType, long long>::value == true,
				std::is_same<IntegerType, unsigned int>::value == true,
				std::is_same<IntegerType, unsigned short>::value == true,
				std::is_same<IntegerType, unsigned long>::value == true,
				std::is_same<IntegerType, unsigned long long>::value == true,
			};

			int current_condition = 0;
			for (current_condition = 0; current_condition < sizeof(conditions) / sizeof(bool); current_condition++)
			{
				if (conditions[current_condition] == true) {
					std::random_device rd;
					IntegerType cmin = min;
					IntegerType cmax = max;
					if (cmin > cmax) {
						cmin = max;
						cmax = min;
					}

					std::uniform_int_distribution<IntegerType> dist(min, max);
					result = dist(rd);
					break;
				}
			}

			return result;
		}

		template <typename FloatType>
		inline FloatType randomFloat(FloatType min, FloatType max) {
			FloatType result = 0;

			constexpr bool conditions[] = {
				std::is_same<FloatType, float>::value == true,
				std::is_same<FloatType, double>::value == true,
				std::is_same<FloatType, long double>::value == true,
			};

			int current_condition = 0;
			for (current_condition = 0; current_condition < sizeof(conditions) / sizeof(bool); current_condition++)
			{
				if (conditions[current_condition] == true) {
					std::random_device rd;
					FloatType cmin = min;
					FloatType cmax = max;
					if (cmin > cmax) {
						cmin = max;
						cmax = min;
					}

					std::uniform_real_distribution<FloatType> dist(min, max);
					result = dist(rd);
					break;
				}
			}

			return result;
		}

		template <typename IntegerType, typename OutputType = int>
		OutputType GetNumbersOfDigit(IntegerType number)
		{
			OutputType digit = 0;

			IntegerType tenmultipler = 1;
			if (math::absolute(number) >= 10) {
				while (tenmultipler <= math::absolute(number)) {
					tenmultipler = tenmultipler * 10;
					digit++;
				}
				tenmultipler = tenmultipler;
			}
			else {
				digit = 1;
			}

			return digit;
		}

		template <typename IntegerType>
		inline int GetIntegerDigit(IntegerType number, int position)
		{
			int result = 0;
			IntegerType NumberToCount = absolute(number);

			constexpr bool conditions[] = {
				std::is_same<IntegerType, int>::value == true,
				std::is_same<IntegerType, short>::value == true,
				std::is_same<IntegerType, long>::value == true,
				std::is_same<IntegerType, long long>::value == true,
				std::is_same<IntegerType, unsigned int>::value == true,
				std::is_same<IntegerType, unsigned short>::value == true,
				std::is_same<IntegerType, unsigned long>::value == true,
				std::is_same<IntegerType, unsigned long long>::value == true,
			};

			int current_condition = 0;
			for (current_condition = 0; current_condition < sizeof(conditions) / sizeof(bool); current_condition++) {
				if (conditions[current_condition] == true) {

					unsigned int digits = 0;
					unsigned int digitspos = 0;

					if (digitspos <= 0) digitspos = 1;
					while ((unsigned int)digitspos < (unsigned int)NumberToCount) {
						digitspos = digitspos * 10;
						digits += 1;
					};

					int* numberfromdigits = new int[digits];

					for (unsigned int i = 0; i < digits; i++) {
						if (digitspos <= 0) break;
						digitspos = digitspos / 10;
						IntegerType NumberToSortDigit = NumberToCount / static_cast<IntegerType>(digitspos);
						while (NumberToSortDigit > 10) {
							for (IntegerType ia = 1; ia <= static_cast<IntegerType>(i); ia++) {
								if ((IntegerType)pow(10, ia) <= (IntegerType)NumberToSortDigit) {
									NumberToSortDigit -= (IntegerType)pow(10, ia);
								}
							}
							if (NumberToSortDigit < 10) {
								goto insert_digit;
							}
						}
					insert_digit:
						//std::cout << "Digit: " << NumberToSortDigit << std::endl;
						if (NumberToSortDigit == 10) NumberToSortDigit -= 10;
						numberfromdigits[i] = (int)NumberToSortDigit;
					}

					if ((unsigned int)position <= (unsigned int)digits) {
						if (numberfromdigits[position - 1]) {
							result = numberfromdigits[position - 1];
						}
					}

					if (numberfromdigits != nullptr) {
						delete[] numberfromdigits;
					}
					break;
				}
			}
			return result;
		}

		template <typename FloatType>
		int GetFloatDigit(FloatType num, unsigned int position, unsigned int DecimalSidePlace)
		{
			int result = 0;
			unsigned int NumberToCount = static_cast<unsigned int>(absolute(num) * pow(10, DecimalSidePlace));

			constexpr bool conditions[] = {
				std::is_same<FloatType, float>::value == true,
				std::is_same<FloatType, double>::value == true,
				std::is_same<FloatType, long double>::value == true,
			};

			int current_condition = 0;
			for (current_condition = 0; current_condition < sizeof(conditions) / sizeof(bool); current_condition++) {
				if (conditions[current_condition] == true) {
					unsigned int digits = 0;
					unsigned int digitspos = 0;

					if (digitspos <= 0) digitspos = 1;
					while (digitspos < NumberToCount) {
						digitspos = digitspos * 10;
						digits += 1;
						//std::cout << "digitspos: " << digitspos << std::endl;
					};

					int* numberfromdigits = new int[digits];


					for (unsigned int i = 0; i < digits; i++) {
						if (digitspos <= 0) break;
						digitspos = digitspos / 10;
						int NumberToSortDigit = (int)(NumberToCount / digitspos);
						while (NumberToSortDigit >= 10) {
							for (unsigned int ia = 1; ia <= i; ia++) {
								if ((long)pow(10, ia) <= (long)NumberToSortDigit) {
									NumberToSortDigit -= (long)pow(10, ia);
								}
							}
							if (NumberToSortDigit < 10) {
								goto insert_digit;
							}
						}
					insert_digit:
						//std::cout << "Digit: " << NumberToSortDigit << std::endl;
						numberfromdigits[i] = (int)NumberToSortDigit;
					}

					if (position <= digits) {
						if (numberfromdigits[position - 1]) {
							result = numberfromdigits[position - 1];
						}
					}

					if (numberfromdigits != nullptr) {
						delete[] numberfromdigits;
					}
					break;
				}
			}
			return result;
		}
	}

	namespace string
	{
		template <typename StringType = wchar_t, typename OutputNumberType = int>
		OutputNumberType length(StringType* string) {
			OutputNumberType len = 0;
			if (string == nullptr) return -1;

			while (*string) {
				len += 1;
				string++;
			}

			return len;
		}

		template <typename StringType = wchar_t>
		StringType** split(StringType* str, StringType delimiter, int* count = nullptr) {
			int numTokens = 1; // At least one token
			int len = 0;

			// Calculate length of string and count number of tokens
			while (str[len] != '\0') {
				if (str[len] == delimiter) {
					numTokens++;
				}
				len++;
			}

			// Allocate memory for array of pointers to tokens
			StringType** tokens = new StringType * [numTokens];
			if (tokens == nullptr) {
				*count = 0;
				return nullptr; // Memory allocation failed
			}

			int tokenIndex = 0;
			int tokenStart = 0;

			// Extract tokens
			for (int i = 0; i <= len; i++) {
				if (str[i] == delimiter || str[i] == '\0') {
					int tokenLength = i - tokenStart;
					tokens[tokenIndex] = new StringType[tokenLength + 1];
					if (tokens[tokenIndex] == nullptr) {
						// Memory allocation failed, free previously allocated memory
						for (int j = 0; j < tokenIndex; j++) {
							delete[] tokens[j];
						}
						delete[] tokens;
						*count = 0;
						return nullptr;
					}
					// Copy token into allocated memory
					for (int k = 0; k < tokenLength; k++) {
						tokens[tokenIndex][k] = str[tokenStart + k];
					}
					tokens[tokenIndex][tokenLength] = '\0'; // Null-terminate the token
					tokenStart = i + 1;
					tokenIndex++;
				}
			}

			if (count != nullptr) {
				*count = numTokens;
			}

			return tokens;
		}
	}

	template <typename InputNumberType = int, typename OutputStringType = wchar_t>
	OutputStringType* tn_tostring(InputNumberType number) noexcept {
		InputNumberType digit = 0;

		InputNumberType tenmultipler = 1;
		if (math::absolute(number) >= 10) {
			while (tenmultipler <= math::absolute(number)) {
				tenmultipler = tenmultipler * 10;
				digit++;
			}
			tenmultipler = tenmultipler;
		}
		else {
			digit = 1;
		}

		InputNumberType isNegative = 0;
		if (number < 0) {
			isNegative = 1;
		}
		InputNumberType totalindextoallocate = digit + isNegative;
		OutputStringType* buffer = new OutputStringType[totalindextoallocate];

		InputNumberType currentnumber = math::absolute(number);
		InputNumberType divisionmultiplier = tenmultipler;
		InputNumberType numbertoremove = math::absolute(number) / divisionmultiplier;
		InputNumberType numbercounted = numbertoremove;

		if (number < 0) {
			buffer[0] = static_cast<OutputStringType>('-');
		}

		InputNumberType convertcount = isNegative;
		while (convertcount < totalindextoallocate) {
			if (divisionmultiplier >= 10) {
				divisionmultiplier = divisionmultiplier / 10;
				currentnumber = math::absolute(number) / divisionmultiplier;
				while (currentnumber >= 10) {
					for (InputNumberType multicount = 1; multicount <= divisionmultiplier; multicount = multicount * 10) {
						if (pow(10, multicount) <= currentnumber) {
							currentnumber -= static_cast<InputNumberType>(pow(static_cast<InputNumberType>(10), multicount));
						}
					}
				}

				numbercounted = currentnumber;
			}

			if (numbercounted == 0) {
				buffer[convertcount] = static_cast<OutputStringType>('0');
			}
			else if (numbercounted == 1) {
				buffer[convertcount] = static_cast<OutputStringType>('1');
			}
			else if (numbercounted == 2) {
				buffer[convertcount] = static_cast<OutputStringType>('2');
			}
			else if (numbercounted == 3) {
				buffer[convertcount] = static_cast<OutputStringType>('3');
			}
			else if (numbercounted == 4) {
				buffer[convertcount] = static_cast<OutputStringType>('4');
			}
			else if (numbercounted == 5) {
				buffer[convertcount] = static_cast<OutputStringType>('5');
			}
			else if (numbercounted == 6) {
				buffer[convertcount] = static_cast<OutputStringType>('6');
			}
			else if (numbercounted == 7) {
				buffer[convertcount] = static_cast<OutputStringType>('7');
			}
			else if (numbercounted == 8) {
				buffer[convertcount] = static_cast<OutputStringType>('8');
			}
			else if (numbercounted == 9) {
				buffer[convertcount] = static_cast<OutputStringType>('9');
			}
			else {
				break;
			}

			convertcount++;
		}

		buffer[totalindextoallocate] = static_cast<OutputStringType>('\0');

		return buffer;
	}

	template <typename InputStringType = wchar_t, typename OutputNumberType = int>
	OutputNumberType tn_tonumber(InputStringType* string) {
		OutputNumberType len = string::length<InputStringType, OutputNumberType>(string);
		OutputNumberType multiplier = 1;
		OutputNumberType buffer = 0;

		OutputNumberType isNegative = 0;
		if (string[0] == static_cast<InputStringType>('-')) {
			isNegative = 1;
		}

		bool zeroesbeforeothers = true;

		for (int digit = isNegative; digit < len; digit++) {
			if (string[digit] == static_cast<InputStringType>('0')) {
				if (zeroesbeforeothers == true) {
					continue;
				}
			}
			else if (string[digit] == static_cast<InputStringType>('1')) {
				buffer += 1;
			}
			else if (string[digit] == static_cast<InputStringType>('2')) {
				buffer += 2;
			}
			else if (string[digit] == static_cast<InputStringType>('3')) {
				buffer += 3;
			}
			else if (string[digit] == static_cast<InputStringType>('4')) {
				buffer += 4;
			}
			else if (string[digit] == static_cast<InputStringType>('5')) {
				buffer += 5;
			}
			else if (string[digit] == static_cast<InputStringType>('6')) {
				buffer += 6;
			}
			else if (string[digit] == static_cast<InputStringType>('7')) {
				buffer += 7;
			}
			else if (string[digit] == static_cast<InputStringType>('8')) {
				buffer += 8;
			}
			else if (string[digit] == static_cast<InputStringType>('9')) {
				buffer += 9;
			}
			else {
				break;
			}

			zeroesbeforeothers = false;
			if (digit < len - 1) {
				buffer *= 10;
			}
		}

		if (isNegative == 1) {
			buffer = buffer * (-1);
		}

		return buffer;
	}

	template <typename InputNumberType, typename OutputNumberType>
	OutputNumberType ConvertNumberType(InputNumberType Input) {
		return static_cast<OutputNumberType>(Input);
	}

	//Dùng để lấy hai hay nhiều lỗi trong vài hàm của GraphicsInterface
	struct FunctionError {
		int NumberCode;
	};
	constexpr void WriteError(FunctionError* error, int NumberCode) {
		if (error != nullptr) {
			error->NumberCode = NumberCode;
		}
	}

	template <typename Resource>
	void SafeDelete(Resource** sr) {
		if (*sr)
		{
			delete (*sr);
		}
	}

	template <class SystemResource>
	void SafeReleaseWinSys(SystemResource** sr) {
		if (*sr)
		{
			(*sr)->Release();
			*sr = nullptr;
		}
	}

	float BalanceBetweenZeroAndOne(float value);
	
#define ByteCopy_Success 0
#define ByteCopy_Fail -1
	int ByteCopy(const void* Source, void* Destination, size_t SizeOfSource);

	template<typename T>
	struct RemoveReference {
		using Type = T;
	};

	template<typename T>
	struct RemoveReference<T&> {
		using Type = T;
	};

	template<typename T>
	struct RemoveReference<T&&> {
		using Type = T;
	};

	template<typename T>
	constexpr typename RemoveReference<T>::Type&& MoveElement(T&& t) noexcept {
		return static_cast<typename RemoveReference<T>::Type&&>(t);
	}

	template<typename T>
	constexpr typename RemoveReference<T>::Type& AsReference(T& t) noexcept {
		return static_cast<typename RemoveReference<T>::Type&>(t);
	}
}