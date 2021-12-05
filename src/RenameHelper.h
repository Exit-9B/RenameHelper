#pragma once

#include "ScriptCommand.h"

class RenameHelper
{
public:
	~RenameHelper();
	RenameHelper(const RenameHelper& other) = delete;
	RenameHelper(RenameHelper&& other) = delete;
	RenameHelper& operator=(const RenameHelper& other) = delete;
	RenameHelper& operator=(RenameHelper&& other) = delete;

	static auto GetSingleton() -> RenameHelper*;

	void InstallHooks();

	void ReadConsoleCommands();

	void ReadSettings();

private:
	RenameHelper();

	void ReadCommands(
		RE::ScriptCommand* a_commands,
		const char* a_maxName,
		std::string_view a_prefix);

	void NameAddr(std::uintptr_t a_ptr, std::string_view a_name);

	static void RenamePapyrusFunc(
		RE::BSScript::Internal::VirtualMachine* a_vm,
		RE::BSScript::NF_util::NativeFunctionBase* a_nativeFunc);

	inline static REL::Relocation<decltype(RenamePapyrusFunc)> _RegisterPapyrusFunc;

	std::ofstream _fs{ "IDARename.py"s };
};
