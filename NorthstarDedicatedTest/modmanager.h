#pragma once
#include "convar.h"
#include <string>
#include <vector>
#include <filesystem>
#include "rapidjson/document.h"

namespace fs = std::filesystem;

const fs::path MOD_FOLDER_PATH = "R2Northstar/mods";
const fs::path MOD_OVERRIDE_DIR = "mod";

const fs::path COMPILED_ASSETS_PATH = "R2Northstar/runtime/compiled";

struct ModConVar
{
public:
	std::string Name;
	std::string DefaultValue;
	std::string HelpString;
	int Flags;
};

struct ModScriptCallback
{
public:
	// would've liked to make it possible to hook arbitrary codecallbacks, but couldn't find a function that calls some ui ones
	//std::string HookedCodeCallback;

	Context Context;

	// called before the codecallback is executed
	std::string BeforeCallback;
	// called after the codecallback has finished executing
	std::string AfterCallback;
};

struct ModScript
{
public:
	std::string Path;
	std::string RsonRunOn;

	std::vector<ModScriptCallback> Callbacks;
};

class Mod
{
public:
	// runtime stuff
	fs::path ModDirectory;
	bool Enabled = true;

	// mod.json stuff:

	// the mod's name
	std::string Name;
	// the mod's description
	std::string Description;
	// the mod's version, should be in semver
	std::string Version;
	// a download link to the mod, for clients that try to join without the mod
	std::string DownloadLink;

	// whether clients need the mod to join servers running this mod
	bool RequiredOnClient;
	// the priority for this mod's files, mods with prio 0 are loaded first, then 1, then 2, etc
	int LoadPriority;

	// custom scripts used by the mod
	std::vector<ModScript> Scripts;
	// convars created by the mod
	std::vector<ModConVar*> ConVars;
	// custom localisation files created by the mod
	std::vector<std::string> LocalisationFiles;

	// other files:

	std::vector<std::string> Vpks;
	std::vector<std::string> KeyValues;
	std::vector<size_t> KeyValuesHash; // size_t because we hash these filesnames: faster than string comp

	// other stuff

	bool wasReadSuccessfully = false;

public:
	Mod(fs::path modPath, char* jsonBuf);
};

struct ModOverrideFile
{
public:
	Mod* owningMod;
	fs::path path;
};

class ModManager
{
private:
	bool m_hasLoadedMods = false;
	bool m_hasEnabledModsCfg;
	rapidjson::Document m_enabledModsCfg;

public:
	std::vector<Mod> m_loadedMods;
	std::unordered_map<std::string, ModOverrideFile> m_modFiles;

public:
	ModManager();
	void LoadMods();
	void UnloadMods();
	void CompileAssetsForFile(const char* filename);

	// compile asset type stuff, these are done in files under Mods/Compiled/
	void BuildScriptsRson();
	void TryBuildKeyValues(const char* filename);
};

void InitialiseModManager(HMODULE baseAddress);

extern ModManager* g_ModManager;