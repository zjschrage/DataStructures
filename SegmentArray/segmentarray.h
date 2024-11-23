#pragma once

#include <stdint.h>
#include <stddef.h>
#include <functional>
#include <limits>

namespace ZLib {

    // Heap allocated memory block (memBlock_):
    // [Header|Block|Block|...|Block]
    // 
    // Header:
    // [ BlockSize|NumBlocks|BitmapSize|HeaderSize|AllocSize|
    //   BlocksInUse|BlockBitMapPtr|BlockOrderingPtr|
    //   BlockBitMap|BlockBitMap|...|BlockBitMap|
    //   FirstBlockPos|SecondBlockPos|...|LasBlockPos ]
    //
    // Block:
    // [ BlockItemsInUse|Item|Item|Item|...|Item]

    template <typename T, typename C = std::less<>>
    class SegmentArray {
    private:

        using Byte = uint8_t;

        struct Constants {
            static constexpr uint32_t ELEMENTS_PER_BLOCK = 8;
            static constexpr uint32_t DEFAULT_NUM_BLOCKS = 8;
            static constexpr uint8_t BITS_IN_BYTE = 8;
            static constexpr uint32_t INVALID = std::numeric_limits<uint32_t>::max();
        };

        struct HeaderMetaData {
            uint32_t blockSize_;
            uint32_t numBlocks_;
            uint32_t blockBitmapSize_;
            uint32_t headerSize_;
            uint32_t allocSize_;
        };

        struct Header {
            HeaderMetaData staticData_;
            uint32_t blocksInUse_{0};
            Byte* blockBitMap_;
            uint32_t* blockOrdering_;
        };

        struct Block {
            uint32_t elementsInUse_{0};
            T elements_[Constants::ELEMENTS_PER_BLOCK];
        };

    public:
        SegmentArray();
        SegmentArray(uint32_t capacity);
        SegmentArray(uint32_t capacity, C comparator);
        ~SegmentArray();

        void EmplaceBack(T&& e);
        void EmplaceFront(T&& e);
        void EmplaceInto(T&& e, uint32_t idx);
        void EmplaceSort(T&& e);

        void DumpMemory();

    private:
        void SlabAllocate(HeaderMetaData metaData);
        void SlabReallocate();
        uint32_t ReserveBlock();
        void FreeBlock(uint32_t id);

    private:
        Byte* memBlock_;
        Header* header_;
        Block* dataBlock_;
        C comparator_;
    };

}

#include "segmentarray.tpp"