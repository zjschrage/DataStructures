#include "segmentarray.h"

int main() {
    ZLib::SegmentArray<int> s;
    s.DumpMemory();
    s.EmplaceBack(1);
    s.EmplaceBack(2);
    s.EmplaceBack(3);
    s.EmplaceBack(4);
    s.EmplaceBack(5);
    s.EmplaceBack(6);
    s.EmplaceBack(7);
    s.EmplaceBack(8);
    s.EmplaceBack(9);
    s.DumpMemory();
}