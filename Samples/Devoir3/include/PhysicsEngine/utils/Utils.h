#ifndef UTILS_H
#define UTILS_H

#define JPH_ENABLE_ASSERTS

#include <cstdarg>
#include <format>
#include <iostream>

namespace Utils
{
	static void TraceImpl(const char* inFMT, ...)
	{
		va_list args;
		va_start(args, inFMT);
		const std::string out = std::vformat(inFMT, std::make_format_args(args));
		va_end(args);

		std::cout << out << '\n';
	}

#ifdef JPH_ENABLE_ASSERTS
	static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, const JPH::uint inLine)
	{
		std::cout << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr ? inMessage : "") << '\n';

		return true;
	};

#endif
}

#endif