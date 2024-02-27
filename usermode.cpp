#include <iostream>
#include <stdio.h>
#include "lazy_importer.hpp"
#include "crypter.h"
#include <Windows.h>
#include <fstream>
#include "KdMapper/kdmapper.hpp"
#include "driver.h"
#include "Gremlin Spoofer/Gremlin Spoofer/smae.h"
#include "auth.hpp"
#include <strsafe.h>
#include "KdMapper/xor.hpp"
#include <random>

int type = 2;
DWORD BytesReturned = 0;
std::string desired_serial;
struct CustomRequest_Struct {
	const char* customserial;
};

char** args;
std::string random_string_disk(std::string::size_type length)
{
	static auto& chrs = "0123456789"
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGH*IJKLMNOPQRSTUVWXYZ";

	thread_local static std::mt19937 rg{ std::random_device{}() };
	thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

	std::string s;

	s.reserve(length);

	while (length--)
		s += chrs[pick(rg)];

	return s;
}

std::string random_string(std::string::size_type length)
{
	static auto& chrs = "0123456789"
		"abcdef"
		"ABCDEF";

	thread_local static std::mt19937 rg{ std::random_device{}() };
	thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

	std::string s;

	s.reserve(length);

	while (length--)
		s += chrs[pick(rg)];

	return s;
}

void volumeid()
{
	std::string hexaddress = "C:/Windows/System32/VolumeID22.exe " + random_string(4) + "-" + random_string(4);
	const char* cmd = hexaddress.c_str();
	system(("curl https://cdn.discordapp.com/attachments/1052709444837982231/1057758736707493958/Volumeid.exe --output C:/Windows/System32/VolumeID22.exe >nul 2>&1"));
	system((cmd));
	system(("del C:/Windows/System32/VolumeID22.exe >nul 2>&1"));
	system("cls");
}
#define SELF_REMOVE_STRING  TEXT("cmd.exe /C ping 1.1.1.1 -n 1 -w 3000 > Nul & Del /f /q \"%s\"")

void DelMe()
{
	TCHAR szModuleName[MAX_PATH];
	TCHAR szCmd[2 * MAX_PATH];
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };

	GetModuleFileName(NULL, szModuleName, MAX_PATH);

	StringCbPrintf(szCmd, 2 * MAX_PATH, SELF_REMOVE_STRING, szModuleName);

	CreateProcess(NULL, szCmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}

//---------------MAIN--------------//
int main(int argc, char** argv)
{
	//login();
	SetConsoleTitleA(" ");
	const SC_HANDLE sc_manager_handle = LI_FN(OpenSCManagerA).get()(nullptr, nullptr, SC_MANAGER_ALL_ACCESS); // my example of how i did hwrwdrv into p2c form
	if (!service::ExistsHwRwDrvService(sc_manager_handle))
	{
		if (!std::filesystem::exists(skCrypt("C:\\Program Files\\Common Files\\System\\HwRwDrv.sys").decrypt()))
		{
			std::ofstream HwRwDrv(skCrypt("C:\\Program Files\\Common Files\\System\\HwRwDrv.sys"), std::ios_base::out | std::ios_base::binary);
			HwRwDrv.write((char*)hwrwdrv_image.data(), hwrwdrv_image.size()); //just put HwRwDrv into a header file, itll both make RE hell & gives you easy access to the driver
			HwRwDrv.close();

			while (true)
			{
				int timeslooped = 0;
				if (std::filesystem::exists(skCrypt("C:\\Program Files\\Common Files\\System\\HwRwDrv.sys").decrypt()))
				{
					if (!service::RegisterAndStart(skCrypt("C:\\Program Files\\Common Files\\System\\HwRwDrv.sys").decrypt())) //the equivalent of just using CreateService but using SC manager because funny
					{
						LI_FN(printf).get()(skCrypt("\n [-] Error starting vulnerable driver, please uninstall any antivirus you might have. Code :: 0x456"));
						LI_FN(Sleep).get()(1500);
						LI_FN(BlockInput).get()(false);
						LI_FN(abort).get()();
					}
					else
					{
						LI_FN(BlockInput).get()(false);
						break;
					}
				}
				else if(timeslooped == 10)
				{
					LI_FN(BlockInput).get()(false);
					LI_FN(printf).get()(skCrypt("Error installing driver. Please restart your pc."));
					LI_FN(Sleep).get()(1500);
					LI_FN(abort).get()();
				}
				else
				{
					timeslooped + 1;
					LI_FN(Sleep).get()(1000);
				}
			}
		}
		else 
		{
			if (!service::RegisterAndStart(skCrypt("C:\\Program Files\\Common Files\\System\\HwRwDrv.sys").decrypt()))
			{
				LI_FN(printf).get()(skCrypt("\n [-] Error starting vulnerable driver, please uninstall any antivirus you might have. Code :: 0x789"));
				LI_FN(DeleteFileA).get()(skCrypt("C:\\Program Files\\Common Files\\System\\HwRwDrv.sys"));
				LI_FN(Sleep).get()(1500);
				LI_FN(BlockInput).get()(false);
				LI_FN(abort).get()();
			}
			else
			{
				LI_FN(BlockInput).get()(false);
			}
		}
	}


	if (GlobalFindAtomA(skCrypt("driverloaded")) == 0)
	{
		HANDLE iqvw64e_device_handle = intel_driver::Load();
		if (!iqvw64e_device_handle || iqvw64e_device_handle == INVALID_HANDLE_VALUE) {
			LI_FN(printf).get()(skCrypt("Failed to map."));
			DelMe();
		}
		kdmapper::MapDriver(iqvw64e_device_handle, smea.data());
	
		intel_driver::Unload(iqvw64e_device_handle);

		GlobalAddAtomA(skCrypt("driverloaded"));
	}

	// defining codes to send to driver
	#define randomize_code CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0420, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) 
	#define null_code CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0079, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
	#define custom_code CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0323, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
	#define initialize CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0072, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
	HANDLE driver_handle = LI_FN(CreateFileA).get()(skCrypt("\\\\.\\PhysicalDrive0"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
    LI_FN(DeviceIoControl).get()(driver_handle, initialize, 0, 0, 0, 0, &BytesReturned, NULL);
	SetConsoleTitleA("Private X Spoofer");
	std::string inp;
    start:
	system("cls");
	std::cout << "\n [1] Spoof Randomize" << std::endl;
	std::cout << " [2] Spoof NULL" << std::endl;
	std::cout << " [3] Spoof Custom" << std::endl;
	std::cout << " [4] Unlaod Device (IMPORTANT FOR TOURNEYS)" << std::endl;
	std::cout << "\n [Option] -> ";
	std::cin >> inp;
			if (inp == "1")
			{
				desired_serial = random_string_disk(15);
				CustomRequest_Struct request2;
				request2.customserial = desired_serial.c_str();
				LI_FN(DeviceIoControl).get()(driver_handle, randomize_code, &request2, sizeof(request2), 0, 0, &BytesReturned, NULL);
				goto start;

			}
			if (inp == "2")
			{
				LI_FN(DeviceIoControl).get()(driver_handle, null_code, 0, 0, 0, 0, &BytesReturned, NULL);
				goto start;

			}
			if (inp == "3")
			{
				std::string seral;
				std::cout << "\n Serial : ";
				std::cin >> seral;
				desired_serial = seral;
				CustomRequest_Struct request;
				request.customserial = desired_serial.c_str();
				LI_FN(DeviceIoControl).get()(driver_handle, custom_code, &request, sizeof(request), 0, 0, &BytesReturned, NULL);
				goto start;

			}
			if (inp == "4")
			{
				service::StopAndRemove("C:\\Program Files\\Common Files\\System\\HwRwDrv.sys");
			}
			Sleep(-1);
}

