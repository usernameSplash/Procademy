#pragma once

#define MAX_CAPACITY 32768
#define DEFAULT_CAPACITY 8192
namespace MyDataStructure
{
    class RingBuffer
    {
    public:
        RingBuffer(void);
        RingBuffer(size_t capacity);

        bool Reserve(size_t capacity);
        size_t Capacity(void);
        size_t Size(void);

        size_t Enqueue(char* srcData, size_t size);
        size_t Dequeue(size_t size);
        size_t Peek(char* dstData, size_t size);
        size_t DirectEnqueueSize(void);
        size_t DirectDequeueSize(void);

        void ClearBuffer(void);
        size_t MoveRear(size_t size);
        size_t MoveFront(size_t size);
        char* GetBufferPtr(void);
        char* GetFrontBufferPtr(void);
        char* GetRearBufferPtr(void);

    private:
        char* mBuffer;
        int mFront;
        int mRear;

        size_t mCapacity;
        size_t mSize;
    };
}