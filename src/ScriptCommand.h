#pragma once

namespace RE
{
	struct ScriptCommand
	{
	public:
		const char*    name;          // 00
		const char*    alias;         // 08
		std::uint32_t  index;         // 10 - script commands start at 0x1000, console-only commands start at 0x100
		std::uint32_t  pad14;         // 14
		const char*    description;   // 18
		bool           useReference;  // 20
		std::uint8_t   pad21;         // 21
		std::uint16_t  numArgs;       // 22
		std::uint32_t  pad24;         // 24
		const char*    argNames;      // 28
		std::uintptr_t handler;       // 30
		std::uintptr_t parser;        // 38
		std::uintptr_t func;          // 40
		std::uint64_t  pad48;         // 48
	};
	static_assert(sizeof(ScriptCommand) == 0x50);
}
