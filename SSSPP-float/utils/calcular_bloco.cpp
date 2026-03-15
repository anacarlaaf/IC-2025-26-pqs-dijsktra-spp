#include <iostream>
#include <cstddef>
#include <new>          // hardware_destructive_interference_size
#include <type_traits>

using namespace std;

// Example heap item (POD, predictable layout)
using keyType = int64_t;

struct HeapItem {
    keyType key;
    int value;
};

static_assert(std::is_standard_layout_v<HeapItem>,
              "HeapItem must have predictable layout");

class CacheInfo {
public:
    static size_t getCacheLineSize() {
        // C++17 standard way
        return std::hardware_destructive_interference_size;
    }

    static void printCacheInfo() {
        size_t cache_line = getCacheLineSize();
        size_t item_size  = sizeof(HeapItem);

        // B-heap formula
        size_t B = (cache_line / item_size) - 1;
        size_t B2 = (196/item_size-1);

        cout << "Cache line size: " << cache_line << " bytes\n";
        cout << "HeapItem size:   " << item_size << " bytes\n";
        cout << "B (children per block): " << B << "\n";
        cout << "B2 " << B2 << "\n";
    }
};

int main() {
    CacheInfo::printCacheInfo();
    return 0;
}
