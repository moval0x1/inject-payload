#include <windows.h>


int main(void) {

	// Payload inside the main function means
	// Local variables = stack
	unsigned char payload[] = {
		0x90,		// nop
		0xb0,		// mov al
		0x01,		// 01
		0xc3		// ret
	};

	// https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc
	// Reserves, commits, or changes the state of a region of pages in the virtual address space of the calling process. 
	// Memory allocated by this function is automatically initialized to zero.
	LPVOID addressPointer = VirtualAlloc(0, sizeof(payload), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	// https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlmovememory
	// The RtlMoveMemory routine copies the contents of a source memory block to a destination memory block, 
	// and supports overlapping source and destination memory blocks.
	RtlMoveMemory(addressPointer, payload, sizeof(payload));

	// https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualprotect
	// Changes the protection on a region of committed pages in the virtual address space of the calling process.
	DWORD oldProtect = 0;
	VirtualProtect(addressPointer, sizeof(payload), PAGE_EXECUTE_READ, &oldProtect);

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
