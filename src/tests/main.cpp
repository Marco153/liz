#include <windows.h>
#include <stdio.h>

int main() {
    printf("Program started. Attempting to handle a breakpoint exception...\n");

    __try {
        printf("Triggering int3 (breakpoint exception)...\n");

        // Trigger the breakpoint exception
        __debugbreak();  // This triggers a breakpoint exception (EXCEPTION_BREAKPOINT)

        printf("This line will not be executed if an exception occurs.\n");
    }
    __except (EXCEPTION_BREAKPOINT == GetExceptionCode() ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        // Get the exception information only within the __except block
        PEXCEPTION_POINTERS exceptionPointers = GetExceptionInformation();
        void* exceptionAddress = exceptionPointers->ExceptionRecord->ExceptionAddress;

        printf("Caught a breakpoint exception (EXCEPTION_BREAKPOINT)!\n");
        printf("Exception occurred at address: %p\n", exceptionAddress);
    }

    printf("Program resumed execution after handling the exception.\n");
    return 0;
}
