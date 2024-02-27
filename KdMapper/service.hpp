#pragma once
#include <Windows.h>
#include <string>
#include <filesystem>
#include "intel_driver.hpp"

namespace service
{
	bool RegisterAndStart(const std::string& driver_path);
	bool RegisterAndStartOnBoot(const std::string& driver_path);
	bool RegisterAndStart(const std::wstring& driver_path);
	bool StopAndRemove(const std::wstring& driver_name);
	bool StopAndRemove(const std::string& driver_name);
	bool ExistsHwRwDrvService(SC_HANDLE service_manager);

};