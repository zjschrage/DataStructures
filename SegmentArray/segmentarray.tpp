#include "segmentarray.h"

#include <iostream>
#include <cmath>

namespace ZLib {
    
    template <typename T, typename C>
    SegmentArray<T, C>::SegmentArray() :
        SegmentArray(Constants::ELEMENTS_PER_BLOCK * Constants::DEFAULT_NUM_BLOCKS)
    { }

    template <typename T, typename C>
    SegmentArray<T, C>::SegmentArray(uint32_t capacity) {
        HeaderMetaData md;
        md.blockSize_ = sizeof(Block);
        md.numBlocks_ = std::ceil(capacity / Constants::ELEMENTS_PER_BLOCK);
        md.blockBitmapSize_ = std::ceil(md.numBlocks_ / Constants::BITS_IN_BYTE);
        md.headerSize_ = sizeof(Header) + md.blockBitmapSize_ + sizeof(Block) * md.numBlocks_;
        md.allocSize_ = md.headerSize_ + (md.blockSize_ * md.numBlocks_);
        SlabAllocate(md);
        ReserveBlock();
    }

    template <typename T, typename C>
    SegmentArray<T, C>::SegmentArray(uint32_t capacity, C comparator) :
        SegmentArray(capacity),
        comparator_(comparator)
    {}

    template <typename T, typename C>
    SegmentArray<T, C>::~SegmentArray() {
        delete[] memBlock_;
    }

    template <typename T, typename C>
    void SegmentArray<T, C>::SlabAllocate(HeaderMetaData md) {
        memBlock_ = new Byte[md.allocSize_];
        
        header_ = reinterpret_cast<Header*>(memBlock_);
        header_->staticData_ = md;
        header_->blocksInUse_ = 0;
        header_->blockBitMap_ = reinterpret_cast<Byte*>(memBlock_ + sizeof(Header));
        header_->blockOrdering_ = reinterpret_cast<uint32_t*>(memBlock_ + sizeof(Header) + md.blockBitmapSize_);
        
        dataBlock_ = reinterpret_cast<Block*>(memBlock_ + md.headerSize_);

#ifdef DEBUG
        printf("Slab Allocated:\n");
        printf("Memblock %p\n", memBlock_);
        printf("Header__ %p\n", header_);
        printf("BitMap__ %p\n", header_->blockBitMap_);
        printf("BlockOrd %p\n", header_->blockOrdering_);
        printf("DataBlok %p\n", dataBlock_);
#endif

        auto* bitmap = header_->blockBitMap_;
        for (size_t i = 0; i < md.blockBitmapSize_; i++) {
            bitmap[i] = 0;
        }

        auto* blockOrdering = header_->blockOrdering_;
        for (size_t i = 0; i < md.numBlocks_; i++) {
            blockOrdering[i] = 0;
        }
    }

    template <typename T, typename C>
    void SegmentArray<T, C>::EmplaceBack(T&& e) {
        auto inUse = header_->blocksInUse_;
        auto lastBlockIdx = header_->blockOrdering_[inUse - 1];
        auto* dataBlock = dataBlock_ + lastBlockIdx;
        auto elementsInUse = dataBlock->elementsInUse_;
        if (elementsInUse >= Constants::ELEMENTS_PER_BLOCK) {
            auto newBlock = ReserveBlock();
            if (newBlock != Constants::INVALID)
            {
                header_->blockOrdering_[header_->blocksInUse_ - 1] = newBlock;
                EmplaceBack(std::move(e));
            }
        }
        else {
            dataBlock->elements_[elementsInUse] = std::move(e);
            dataBlock->elementsInUse_++;
        }
    }

    template <typename T, typename C>
    uint32_t SegmentArray<T, C>::ReserveBlock() {
        auto* bitmap = header_->blockBitMap_;
        const auto bitsInByte = Constants::BITS_IN_BYTE;
        for (size_t i = 0; i < header_->staticData_.blockBitmapSize_; i++) {
            uint8_t mask = 0b10000000;
            for (size_t b = 0; b < bitsInByte; b++) {
                if (!(*bitmap & mask)) {
                    *bitmap |= mask;
                    header_->blocksInUse_++;
                    return bitsInByte * i + b;
                }
                mask = mask >> 1;
            }
            bitmap++;
        }
        return Constants::INVALID;
    }

    template <typename T, typename C>
    void SegmentArray<T, C>::DumpMemory() {
        std::cout << "Header (Static Data):\n";

        const auto& staticData = header_->staticData_;
        std::cout << "Block Size: " << staticData.blockSize_ << "\n";
        std::cout << "Number of Blocks: " << staticData.numBlocks_ << "\n";
        std::cout << "Block Bitmap Size: " << staticData.blockBitmapSize_ << "\n";
        std::cout << "Header Size: " << staticData.headerSize_ << "\n";
        std::cout << "Allocated Size: " << staticData.allocSize_ << "\n";

        std::cout << "Header (Dynamic Data):\n";
        std::cout << "Blocks In Use: " << header_->blocksInUse_ << "\n";

        std::cout << "Block Bitmap:\n";
        auto* bitmapPtr = header_->blockBitMap_;
        for (size_t i = 0; i < staticData.blockBitmapSize_; i++) {
            auto bitmap = *bitmapPtr;
            uint8_t mask = 0b10000000;
            for (size_t b = 0; b < Constants::BITS_IN_BYTE; b++) {
                bool bitFlagOn = (bitmap & mask);
                std::cout << (bitFlagOn ? "1 " : "0 ");
                mask = mask >> 1;
            }
            std::cout << " ";
            bitmapPtr++;
        }
        std::cout << "\n";

        std::cout << "Block Ordering:\n";
        auto* blockOrdering = header_->blockOrdering_;
        for (size_t b = 0; b < staticData.numBlocks_; b++) {
            std::cout << (uint32_t)*blockOrdering << " ";
            blockOrdering++;
        }
        std::cout << "\n";

        std::cout << "Block Element Dump:\n";
        auto* blockData = dataBlock_;
        for (size_t b = 0; b < Constants::DEFAULT_NUM_BLOCKS; b++) {
            for (size_t e = 0; e < blockData->elementsInUse_; e++) {
                std::cout << blockData->elements_[e] << " ";
            }
            for (size_t e = blockData->elementsInUse_; e < Constants::ELEMENTS_PER_BLOCK; e++) {
                std::cout << "_ ";
            }
            blockData++;
            std::cout << "\n";
        }
    }

}