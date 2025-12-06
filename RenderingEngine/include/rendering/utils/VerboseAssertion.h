#ifndef VERBOSE_ASSERTION_H
#define VERBOSE_ASSERTION_H

#include <source_location>
#include <string>

inline void VerboseAssertion(const bool expr, const char* exprStr,
                             const std::string& message = "",
                             const std::source_location& location = std::source_location::current())
{
	if (!expr)
	{
		std::string msg = "Assertion failed: ";
		msg += exprStr;

		if (!message.empty())
		{
			msg += "\n\nMessage: ";
			msg += message;
			msg += '\n';
		}

		msg += "\nIn function: ";
		msg += location.function_name();
		msg += "\nFile: ";
		msg += location.file_name();
		msg += ":";
		msg += std::to_string(location.line());

		// Create a window that displays the message and abort the program
		MessageBoxA(nullptr, msg.c_str(), "Assertion Failed", MB_ICONERROR | MB_OK);
		std::abort();
	}
}

#define vassert(expr, msg) VerboseAssertion((expr), #expr, (msg))

#endif
