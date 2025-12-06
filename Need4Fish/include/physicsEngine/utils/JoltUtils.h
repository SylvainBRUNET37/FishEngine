#ifndef JOLT_UTILS_H
#define JOLT_UTILS_H

#define JPH_ENABLE_ASSERTS

#include <cstdarg>
#include <format>
#include <iostream>
#include "entities/Entity.h"
#include "entities/EntityManager.h"

namespace JoltUtils
{

	void TurnFishAround(EntityManager& entityManager, const Entity& entity);

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
		std::cerr << "Jolt Assertion Failed!\n";
		std::cerr << "Expression: " << inExpression << "\n";
		std::cerr << "Message: " << (inMessage ? inMessage : "(none)") << "\n";
		std::cerr << "File: " << inFile << ":" << inLine << "\n";

		assert(false && "Jolt Assertion Failed. See error output for more information.");
		return false;
	};

#endif
}

#endif