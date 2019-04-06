#pragma once
#include <string>
#include "..\Core\Logger.h"

class Asset
{
public:
	Asset(){};
	virtual ~Asset(){ /*Logger::LogInfo("Delete asset", name);*/ };
	std::string name;
	bool preserve;

};

class InternalAsset : public Asset
{
public:
	InternalAsset(){};
	virtual ~InternalAsset(){ /*Logger::LogInfo("Delete asset", name);*/ };

};