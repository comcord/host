////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design 
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The author or Addison-Wesley Longman make no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// Last update: June 20, 2001
#ifndef UCHAR_MAX
#define UCHAR_MAX     0xff      /* maximum unsigned char value */
#endif //UCHAR_MAX
#ifndef GMSMALLOBJ_INC_
#define GMSMALLOBJ_INC_

//#include "Threads.h"
//#include "GMSingleTon.h"
//#include <cstddef>
#include <vector>

#ifndef DEFAULT_CHUNK_SIZE
#define DEFAULT_CHUNK_SIZE 4096
#endif

#ifndef MAX_SMALL_OBJECT_SIZE
#define MAX_SMALL_OBJECT_SIZE 1024
#endif

    class GMFixedAllocator
    {
    private:
        struct Chunk
        {
            bool Init(std::size_t blockSize, unsigned char blocks);
            void* Allocate(std::size_t blockSize);
            void Deallocate(void* p, std::size_t blockSize);
            void Reset(std::size_t blockSize, unsigned char blocks);
            void Release();
            unsigned char* pData_;
            unsigned char
                firstAvailableBlock_,
                blocksAvailable_;
        };
        
        // Internal functions        
        void DoDeallocate(void* p);
        Chunk* VicinityFind(void* p);
        
        // Data 
        std::size_t blockSize_;
        unsigned char numBlocks_;
        typedef std::vector<Chunk> Chunks;
        Chunks chunks_;
        Chunk* allocChunk_;
        Chunk* deallocChunk_;
        // For ensuring proper copy semantics
        mutable const GMFixedAllocator* prev_;
        mutable const GMFixedAllocator* next_;
        
    public:
        // Create a GMFixedAllocator able to manage blocks of 'blockSize' size
        explicit GMFixedAllocator(std::size_t blockSize = 0);
        GMFixedAllocator(const GMFixedAllocator&);
        GMFixedAllocator& operator=(const GMFixedAllocator&);
        ~GMFixedAllocator();
        
        void Swap(GMFixedAllocator& rhs);
        
        // Allocate a memory block
        void* Allocate();
        // Deallocate a memory block previously allocated with Allocate()
        // (if that's not the case, the behavior is undefined)
        void Deallocate(void* p);
        // Returns the block size with which the GMFixedAllocator was initialized
        std::size_t BlockSize() const
        { return blockSize_; }
    };
    
////////////////////////////////////////////////////////////////////////////////
// class GMSmallObjAllocator
// Offers services for allocating small-sized objects
////////////////////////////////////////////////////////////////////////////////

    class GMSmallObjAllocator
    {
    public:
        GMSmallObjAllocator(
            std::size_t chunkSize, 
            std::size_t maxObjectSize);
    
        void* Allocate(std::size_t numBytes);
        void Deallocate(void* p, std::size_t size);
    
    private:
        GMSmallObjAllocator(const GMSmallObjAllocator&);
        GMSmallObjAllocator& operator=(const GMSmallObjAllocator&);
        
        typedef std::vector<GMFixedAllocator> Pool;
        Pool pool_;
        GMFixedAllocator* pLastAlloc_;
        GMFixedAllocator* pLastDealloc_;
        std::size_t chunkSize_;
        std::size_t maxObjectSize_;
    };
#endif // SMALLOBJ_INC_
