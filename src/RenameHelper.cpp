#include "RenameHelper.h"

RenameHelper::RenameHelper()
{
	_py <<
		"def NameAddr(ea, name) :\n"
		"    idc.set_name(ea, name, idc.SN_AUTO);\n"
		"\n"
		"def TypeAddr(ea, type):\n"
		"	idc.SetType(ea, type)\n"
		"\n"
		"print('Importing names...')\n"
		"\n";
}

RenameHelper::~RenameHelper()
{
	_py <<
		"\n"
		"print('Finished import')\n";
}

auto RenameHelper::GetSingleton() -> RenameHelper*
{
	static RenameHelper singleton{};
	return std::addressof(singleton);
}

void RenameHelper::InstallHooks()
{
	// BSScript::Internal::VirtualMachine::vtbl
	REL::Relocation<std::uintptr_t> vtbl{ REL::ID(252631) };
	_RegisterPapyrusFunc = vtbl.write_vfunc(24, RenamePapyrusFunc);
}

void RenameHelper::ReadConsoleCommands()
{
	auto gameFuncs = RE::SCRIPT_FUNCTION::GetFirstScriptCommand();
	ReadCommands(gameFuncs, "ADD NEW FUNCTIONS BEFORE THIS ONE!!!", "GameFunc"sv);

	auto consoleFuncs = RE::SCRIPT_FUNCTION::GetFirstConsoleCommand();
	ReadCommands(consoleFuncs, " ", "ConsoleFunc"sv);
}

void RenameHelper::ReadSettings()
{
	auto gameSettings = RE::GameSettingCollection::GetSingleton();
	for (auto& item : gameSettings->settings) {
		auto name = item.first;
		auto setting = item.second;

		NameAddr(
			reinterpret_cast<std::uintptr_t>(setting),
			fmt::format("gGameSetting_{}"sv, name));
	}

	auto iniPrefSettings = RE::INIPrefSettingCollection::GetSingleton();
	for (auto& setting : iniPrefSettings->settings) {
		std::string name{ setting->name };
		std::replace(name.begin(), name.end(), ':', '_');
		std::replace(name.begin(), name.end(), ' ', '_');

		NameAddr(
			reinterpret_cast<std::uintptr_t>(setting),
			fmt::format("gIniPref_{}"sv, name));
	}

	auto iniSettings = RE::INISettingCollection::GetSingleton();
	for (auto& setting : iniSettings->settings) {
		std::string name{ setting->name };
		std::replace(name.begin(), name.end(), ':', '_');
		std::replace(name.begin(), name.end(), ' ', '_');

		NameAddr(
			reinterpret_cast<std::uintptr_t>(setting),
			fmt::format("gIni_{}"sv, name));
	}
}

void RenameHelper::ReadCommands(
	RE::SCRIPT_FUNCTION* a_commands,
	const char* a_maxName,
	std::string_view a_prefix)
{
	for (auto command = a_commands;
		command->functionName && strcmp(command->functionName, a_maxName);
		++command) {

		auto funcName = command->functionName;
		while (*funcName == ' ') {
			funcName++;
		}

		if (auto conditionFunction = command->conditionFunction) {
			NameAddr(
				reinterpret_cast<std::uintptr_t>(conditionFunction),
				fmt::format("{}::{}::{}"sv, a_prefix, "native"sv, funcName));
		}

		if (auto executeFunction = command->executeFunction) {
			NameAddr(
				reinterpret_cast<std::uintptr_t>(executeFunction),
				fmt::format("{}::{}::{}"sv, a_prefix, "handler"sv, funcName));
		}
	}
}

void RenameHelper::NameAddr(std::uintptr_t a_address, std::string_view a_name)
{
	auto base = REL::Module::get().base();
	auto textw = REL::Module::get().segment(REL::Segment::Name::textw);

	if (a_address < base || a_address > textw.address() + textw.size()) {
		return;
	}

	auto offset = a_address - base;
	auto ida_address = 0x140000000 + offset;

	_py << fmt::format("NameAddr({0:#X}, '{1}_{0:X}')"sv, ida_address, a_name) << std::endl;
	_txt << fmt::format("{0:X}\t{1}_{0:X}\t"sv, ida_address, a_name) << std::endl;
}

void RenameHelper::RenamePapyrusFunc(
	[[maybe_unused]] RE::BSScript::Internal::VirtualMachine* a_vm,
	RE::BSScript::NF_util::NativeFunctionBase* a_nativeFunc)
{
	auto& funcName = a_nativeFunc->GetName();
	auto& typeName = a_nativeFunc->GetObjectTypeName();
	auto funcPtr = *reinterpret_cast<std::uintptr_t*>(a_nativeFunc + 1);

	auto rh = GetSingleton();
	rh->NameAddr(funcPtr, fmt::format("papyrus::{}::{}"sv, typeName, funcName));

	return _RegisterPapyrusFunc(a_vm, a_nativeFunc);
}
