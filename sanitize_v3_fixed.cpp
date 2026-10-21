#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <climits>

// Global variables for ASAN and TSAN testing
int global_array[10] = {0};
volatile int shared_var = 0;
int shared_array[10] = {0};
volatile int shared_flag = 0;

// ==========================================
// 15 ADDRESSSANITIZER (ASAN) ISSUES
// ==========================================
void trigger_asan_issues() {
    std::cout << "[*] Triggering ASAN issues...\n";

    // 1. Heap buffer overflow (read)
    {
        int* p = new int[10];
        volatile int val = p[15]; 
        (void)val;
        delete[] p;
    }

    // 2. Heap buffer overflow (write)
    {
        int* p = new int[10];
        p[12] = 42; 
        delete[] p;
    }

    // 3. Heap use after free (read)
    {
        int* p = new int[5];
        delete[] p;
        volatile int val = p[0];
        (void)val;
    }

    // 4. Heap use after free (write)
    {
        int* p = new int[5];
        delete[] p;
        p[0] = 100;
    }

    // 5. Stack buffer overflow (read)
    {
        int stack_arr[5];
        volatile int val = stack_arr[10];
        (void)val;
    }

    // 6. Stack buffer overflow (write)
    {
        int stack_arr[5];
        stack_arr[10] = 99;
    }

    // 7. Global buffer overflow (read)
    {
        volatile int val = global_array[15];
        (void)val;
    }

    // 8. Global buffer overflow (write)
    {
        global_array[15] = 123;
    }

    // 9. Memory leak (deliberate allocation without delete - caught on normal exit)
    {
        int* leak = new int[100];
        leak[0] = 42;
        // Intentionally left un-deleted for LeakSanitizer (LSan)
    }

    // 10. Double free
    {
        int* p = new int[10];
        delete[] p;
        delete[] p;
    }

    // 11. Invalid free (freeing stack address)
    {
        int x = 42;
        int* stack_ptr = &x;
        std::free(stack_ptr);
    }

    // 12. Use after scope
    {
        int* ptr = nullptr;
        {
            int local = 42;
            ptr = &local;
        }
        volatile int val = *ptr;
        (void)val;
    }

    // 13. Mismatched new/delete (new[] with single delete)
    {
        int* p = new int[10];
        delete p; 
    }

    // 14. Heap buffer overflow via memcpy
    {
        char* buf = new char[5];
        std::memset(buf, 'A', 10); 
        delete[] buf;
    }

    // 15. Stack buffer overflow via strcpy
    {
        char stack_buf[4];
        char src[] = "TooLong";
        for(int i = 0; i <= 7; ++i) {
            stack_buf[i] = src[i];
        }
    }
}

// ==========================================
// 15 UNDEFINEDBEHAVIORSANITIZER (UBSAN) ISSUES
// ==========================================
void trigger_ubsan_issues() {
    std::cout << "[*] Triggering UBSAN issues...\n";

    // 1. Signed integer overflow
    {
        volatile int a = INT_MAX;
        volatile int b = a + 1;
        (void)b;
    }

    // 2. Shift exponent too large
    {
        volatile int a = 1;
        volatile int shift = 35;
        volatile int b = a << shift;
        (void)b;
    }

    // 3. Negative shift exponent
    {
        volatile int a = 1;
        volatile int shift = -1;
        volatile int b = a << shift;
        (void)b;
    }

    // 4. Division by zero
    {
        volatile int a = 10;
        volatile int b = 0;
        volatile int c = a / b;
        (void)c;
    }

    // 5. Modulo by zero
    {
        volatile int a = 10;
        volatile int b = 0;
        volatile int c = a % b;
        (void)c;
    }

    // 6. Negation overflow (abs of INT_MIN)
    {
        volatile int a = INT_MIN;
        volatile int b = -a;
        (void)b;
    }

    // 7. Float to int overflow
    {
        volatile float f = 1e20f;
        volatile int i = static_cast<int>(f);
        (void)i;
    }

    // 8. Load of misaligned pointer
    {
        char arr[sizeof(int) + 2] = {0};
        // Force unaligned pointer reinterpret cast
        int* p = reinterpret_cast<int*>(arr + 1); 
        volatile int val = *p;
        (void)val;
    }

    // 9. Reference binding to null pointer
    {
        int* p = nullptr;
        volatile int& ref = *p; 
        (void)ref;
    }

    // 10. Pointer overflow
    {
        char* base = reinterpret_cast<char*>(0x1000);
        char* p2 = base + -10; 
        (void)p2;
    }

    // 11. Bitwise shift of negative value
    {
        volatile int a = -1;
        volatile int b = a << 2;
        (void)b;
    }

    // 12. Enum value out of range
    {
        enum class Color : unsigned char { Red = 1, Green = 2 };
        volatile int invalid_val = 255;
        Color c = static_cast<Color>(invalid_val); 
        (void)c;
    }

    // 13. Multiplication overflow
    {
        volatile int a = 1000000;
        volatile int b = 1000000;
        volatile int c = a * b;
        (void)c;
    }

    // 14. Subtraction overflow
    {
        volatile int a = INT_MIN;
        volatile int b = 10;
        volatile int c = a - b;
        (void)c;
    }

    // 15. Division overflow (INT_MIN / -1)
    {
        volatile int a = INT_MIN;
        volatile int b = -1;
        volatile int c = a / b;
        (void)c;
    }
}

// ==========================================
// 3 THREADSANITIZER (TSAN) ISSUES
// ==========================================
void trigger_tsan_issues() {
    std::cout << "[*] Triggering TSAN issues...\n";

    std::thread t1([]() {
        for (int i = 0; i < 1000; ++i) shared_var++;
    });

    std::thread t2([]() {
        for (int i = 0; i < 1000; ++i) shared_var++;
    });

    std::thread t3([]() {
        for (int i = 0; i < 100; ++i) shared_array[5] = i;
    });

    for (int i = 0; i < 100; ++i) {
        volatile int val = shared_array[5];
        (void)val;
    }

    std::thread t4([]() {
        shared_flag = 42;
    });

    volatile int flag_val = shared_flag;
    (void)flag_val;

    t1.join();
    t2.join();
    t3.join();
    t4.join();
}

int main(int argc, char* argv[]) {
    if (argc > 1 && std::strcmp(argv[1], "tsan") == 0) {
        trigger_tsan_issues();
    } else {
        trigger_asan_issues();
        trigger_ubsan_issues();
    }
    std::cout << "[*] Execution completed successfully.\n";
    return 0;
}// C++ source file
