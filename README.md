# DLL Injection Project

This repository contains a C program that demonstrates a **DLL Injection** technique to execute shellcode in a remote process.

## Files Included:
- `shellcode_injector.c`: A C file that performs DLL injection and executes shellcode in the target process.
- `injected_shellcode.dll`: A DLL that contains the shellcode to be injected into the target process.

## How It Works:
The `shellcode_injector.c` file allocates memory in the target process, copies the shellcode to that memory, and creates a remote thread to execute it. The `injected_shellcode.dll` contains the shellcode that opens a reverse shell back to the attacker machine.

## Usage:
1. Compile `shellcode_injector.c` using a C compiler (e.g., GCC or MSVC).
2. Inject `injected_shellcode.dll` into a target process.
3. The shellcode will run in the target process and connect back to the attacker's machine.


## License:
- This project is for educational purposes only. Please use it responsibly.

