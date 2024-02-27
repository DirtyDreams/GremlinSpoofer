#include "service.hpp"
#include "xor.hpp"

bool ExistOtherService(SC_HANDLE service_manager) {
	DWORD spaceNeeded = 0;
	DWORD numServices = 0;
	if (!EnumServicesStatus(service_manager, SERVICE_DRIVER, SERVICE_STATE_ALL, NULL, 0, &spaceNeeded, &numServices, 0) && GetLastError() != ERROR_MORE_DATA) {
		return true;
	}
	spaceNeeded += sizeof(ENUM_SERVICE_STATUSA);
	LPENUM_SERVICE_STATUSA buffer = (LPENUM_SERVICE_STATUSA)new BYTE[spaceNeeded];

	if (EnumServicesStatus(service_manager, SERVICE_DRIVER, SERVICE_STATE_ALL, buffer, spaceNeeded, &spaceNeeded, &numServices, 0)) {
		for (DWORD i = 0; i < numServices; i++) {
			ENUM_SERVICE_STATUSA service = buffer[i];
			SC_HANDLE service_handle = OpenService(service_manager, service.lpServiceName, SERVICE_QUERY_CONFIG);
			if (service_handle) {
				LPQUERY_SERVICE_CONFIGA config = (LPQUERY_SERVICE_CONFIGA)new BYTE[8096]; //8096 = max size of QUERY_SERVICE_CONFIGA
				DWORD needed = 0;
				if (QueryServiceConfig(service_handle, config, 8096, &needed)) {
					if (strstr(config->lpBinaryPathName, intel_driver::driver_name)) {
						delete[] buffer;
						CloseServiceHandle(service_handle);
						return false;
					}
				}
				else {
				}
				CloseServiceHandle(service_handle);
			}
			
		}
		delete[] buffer;
		return false; //no equal services we can continue
	}
	delete[] buffer;
	return true;

}

bool ExistsValorantService(SC_HANDLE service_manager) {
	DWORD spaceNeeded = 0;
	DWORD numServices = 0;
	if (!EnumServicesStatus(service_manager, SERVICE_DRIVER, SERVICE_STATE_ALL, NULL, 0, &spaceNeeded, &numServices, 0) && GetLastError() != ERROR_MORE_DATA) {
		return true;
	}
	spaceNeeded += sizeof(ENUM_SERVICE_STATUSA);
	LPENUM_SERVICE_STATUSA buffer = (LPENUM_SERVICE_STATUSA)new BYTE[spaceNeeded];

	if (EnumServicesStatus(service_manager, SERVICE_DRIVER, SERVICE_STATE_ALL, buffer, spaceNeeded, &spaceNeeded, &numServices, 0)) {
		for (DWORD i = 0; i < numServices; i++) {
			ENUM_SERVICE_STATUSA service = buffer[i];
			if (strstr(service.lpServiceName, _("vgk").c_str())) 
			{
				if ((service.ServiceStatus.dwCurrentState == SERVICE_RUNNING || service.ServiceStatus.dwCurrentState == SERVICE_START_PENDING)) 
				{
					return true;
				}

			}
		}
		delete[] buffer;
		return false; //no valorant service found
	}
	delete[] buffer;
	return true;
}

bool service::RegisterAndStart(const std::string& driver_path)
{
	const std::string driver_name = std::filesystem::path(driver_path).filename().string();
	const SC_HANDLE sc_manager_handle = OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);

	if (!sc_manager_handle) {
		return false;
	}

	SC_HANDLE service_handle = CreateService(sc_manager_handle, driver_name.c_str(), driver_name.c_str(), SERVICE_START | SERVICE_STOP | DELETE, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_IGNORE, driver_path.c_str(), nullptr, nullptr, nullptr, nullptr, nullptr);

	if (!service_handle)
	{
		service_handle = OpenService(sc_manager_handle, driver_name.c_str(), SERVICE_START);

		if (!service_handle)
		{
			CloseServiceHandle(sc_manager_handle);
			return false;
		}
	}

	const bool result = StartService(service_handle, 0, nullptr);

	CloseServiceHandle(service_handle);
	CloseServiceHandle(sc_manager_handle);
	if (!result) {
	}
	return result;
}

bool service::StopAndRemove(const std::string& driver_name)
{
	const SC_HANDLE sc_manager_handle = OpenSCManager(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE);

	if (!sc_manager_handle)
		return false;

	const SC_HANDLE service_handle = OpenService(sc_manager_handle, driver_name.c_str(), SERVICE_STOP | DELETE);

	if (!service_handle)
	{
		CloseServiceHandle(sc_manager_handle);
		return false;
	}

	SERVICE_STATUS status = { 0 };
	const bool result = ControlService(service_handle, SERVICE_CONTROL_STOP, &status) && DeleteService(service_handle);

	CloseServiceHandle(service_handle);
	CloseServiceHandle(sc_manager_handle);

	return result;
}


bool service::RegisterAndStartOnBoot(const std::string& driver_path)
{
	const std::string driver_name = std::filesystem::path(driver_path).filename().string();
	const SC_HANDLE sc_manager_handle = OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);

	if (!sc_manager_handle) {
		return false;
	}
	if (ExistOtherService(sc_manager_handle)) {
		return false;
	}

	if (ExistsValorantService(sc_manager_handle)) {
		return false;
	}

	SC_HANDLE service_handle = CreateService(sc_manager_handle, driver_name.c_str(), driver_name.c_str(), SERVICE_START | SERVICE_STOP | DELETE, SERVICE_KERNEL_DRIVER, SERVICE_BOOT_START, SERVICE_ERROR_IGNORE, driver_path.c_str(), nullptr, nullptr, nullptr, nullptr, nullptr);

	if (!service_handle)
	{
		service_handle = OpenService(sc_manager_handle, driver_name.c_str(), SERVICE_START);

		if (!service_handle)
		{
			CloseServiceHandle(sc_manager_handle);
			return false;
		}
	}

	const bool result = StartService(service_handle, 0, nullptr);

	CloseServiceHandle(service_handle);
	CloseServiceHandle(sc_manager_handle);
	if (!result) {
	}
	return result;
}


bool service::ExistsHwRwDrvService(SC_HANDLE service_manager) {
	DWORD spaceNeeded = 0;
	DWORD numServices = 0;
	if (!EnumServicesStatus(service_manager, SERVICE_DRIVER, SERVICE_STATE_ALL, NULL, 0, &spaceNeeded, &numServices, 0) && GetLastError() != ERROR_MORE_DATA) {
		return true;
	}
	spaceNeeded += sizeof(ENUM_SERVICE_STATUSA);
	LPENUM_SERVICE_STATUSA buffer = (LPENUM_SERVICE_STATUSA)new BYTE[spaceNeeded];

	if (EnumServicesStatus(service_manager, SERVICE_DRIVER, SERVICE_STATE_ALL, buffer, spaceNeeded, &spaceNeeded, &numServices, 0)) {
		for (DWORD i = 0; i < numServices; i++) {
			ENUM_SERVICE_STATUSA service = buffer[i];
			if (strstr(service.lpServiceName, _("HwRwDrv.sys").c_str()))
			{
				if ((service.ServiceStatus.dwCurrentState == SERVICE_RUNNING || service.ServiceStatus.dwCurrentState == SERVICE_START_PENDING))
				{
					return true;
				}

			}
		}
		delete[] buffer;
		return false;
	}
	delete[] buffer;
	return true;
}


bool service::RegisterAndStart(const std::wstring& driver_path) {
	const static DWORD ServiceTypeKernel = 1;
	const std::wstring driver_name = intel_driver::GetDriverNameW();
	const std::wstring servicesPath = L"SYSTEM\\CurrentControlSet\\Services\\" + driver_name;
	const std::wstring nPath = L"\\??\\" + driver_path;

	HKEY dservice;
	LSTATUS status = RegCreateKeyW(HKEY_LOCAL_MACHINE, servicesPath.c_str(), &dservice); //Returns Ok if already exists
	if (status != ERROR_SUCCESS) {
		Log("[-] Can't create service key" << std::endl);
		return false;
	}

	status = RegSetKeyValueW(dservice, NULL, L"ImagePath", REG_EXPAND_SZ, nPath.c_str(), (DWORD)(nPath.size() * sizeof(wchar_t)));
	if (status != ERROR_SUCCESS) {
		RegCloseKey(dservice);
		Log("[-] Can't create 'ImagePath' registry value" << std::endl);
		return false;
	}

	status = RegSetKeyValueW(dservice, NULL, L"Type", REG_DWORD, &ServiceTypeKernel, sizeof(DWORD));
	if (status != ERROR_SUCCESS) {
		RegCloseKey(dservice);
		Log("[-] Can't create 'Type' registry value" << std::endl);
		return false;
	}

	RegCloseKey(dservice);

	HMODULE ntdll = GetModuleHandleA("ntdll.dll");
	if (ntdll == NULL) {
		return false;
	}

	auto RtlAdjustPrivilege = (nt::RtlAdjustPrivilege)GetProcAddress(ntdll, "RtlAdjustPrivilege");
	auto NtLoadDriver = (nt::NtLoadDriver)GetProcAddress(ntdll, "NtLoadDriver");

	ULONG SE_LOAD_DRIVER_PRIVILEGE = 10UL;
	BOOLEAN SeLoadDriverWasEnabled;
	NTSTATUS Status = RtlAdjustPrivilege(SE_LOAD_DRIVER_PRIVILEGE, TRUE, FALSE, &SeLoadDriverWasEnabled);
	if (!NT_SUCCESS(Status)) {
		Log("Fatal error: failed to acquire SE_LOAD_DRIVER_PRIVILEGE. Make sure you are running as administrator." << std::endl);
		return false;
	}

	std::wstring wdriver_reg_path = L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\" + driver_name;
	UNICODE_STRING serviceStr;
	RtlInitUnicodeString(&serviceStr, wdriver_reg_path.c_str());

	Status = NtLoadDriver(&serviceStr);
	Log("[+] NtLoadDriver Status 0x" << std::hex << Status << std::endl);

	//Never should occur since kdmapper checks for "IsRunning" driver before
	if (Status == 0xC000010E) {// STATUS_IMAGE_ALREADY_LOADED
		return true;
	}

	return NT_SUCCESS(Status);
}

bool service::StopAndRemove(const std::wstring& driver_name) {
	HMODULE ntdll = GetModuleHandleA("ntdll.dll");
	if (ntdll == NULL)
		return false;

	std::wstring wdriver_reg_path = L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\" + driver_name;
	UNICODE_STRING serviceStr;
	RtlInitUnicodeString(&serviceStr, wdriver_reg_path.c_str());

	HKEY driver_service;
	std::wstring servicesPath = L"SYSTEM\\CurrentControlSet\\Services\\" + driver_name;
	LSTATUS status = RegOpenKeyW(HKEY_LOCAL_MACHINE, servicesPath.c_str(), &driver_service);
	if (status != ERROR_SUCCESS) {
		if (status == ERROR_FILE_NOT_FOUND) {
			return true;
		}
		return false;
	}
	RegCloseKey(driver_service);

	auto NtUnloadDriver = (nt::NtUnloadDriver)GetProcAddress(ntdll, "NtUnloadDriver");
	NTSTATUS st = NtUnloadDriver(&serviceStr);
	Log("[+] NtUnloadDriver Status 0x" << std::hex << st << std::endl);
	if (st != 0x0) {
		Log("[-] Driver Unload Failed!!" << std::endl);
		status = RegDeleteKeyW(HKEY_LOCAL_MACHINE, servicesPath.c_str());
		return false; //lets consider unload fail as error because can cause problems with anti cheats later
	}


	status = RegDeleteKeyW(HKEY_LOCAL_MACHINE, servicesPath.c_str());
	if (status != ERROR_SUCCESS) {
		return false;
	}
	return true;
}
