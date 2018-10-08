#include <stddef.h>
#include "VncBufferReader.h"
#include "VncDataPackage.h"
#include "VncCommandQueue.h"
#include "VncUtils.h"

VncBufferReader::VncBufferReader(VncCommandQueue * cmdIn)
{
    m_commandIn = cmdIn;
    m_allocSize = 2048;
    m_cachedSize = 0;
    m_cacheBuff = new unsigned char[m_allocSize];
    m_pkgSize = 0;
    m_haveGotSize = false;
}

VncBufferReader::~VncBufferReader()
{
    freeCacheBuffer();
}

void VncBufferReader::freeCacheBuffer()
{
    if (m_cacheBuff)
    {
        delete[] m_cacheBuff;
        m_cacheBuff = nullptr;
    }
}

int VncBufferReader::addBuffer(unsigned char * buffer, unsigned buffsize)
{
    if (buffsize == 0)
        return 0;

    // 如果已分配的缓冲区无法再容纳更多数据,则分配更大空间
    if (m_allocSize < m_cachedSize + buffsize)
    {
        unsigned char * newbuff;                // 新分配缓冲区
        unsigned int  newAllocSize;             // 新分配大小

        newAllocSize = m_cachedSize + buffsize;
        newbuff = new unsigned char[newAllocSize];
        if (m_cachedSize)
            memcpy(newbuff, m_cacheBuff, m_cachedSize);
        delete[] m_cacheBuff;

        m_allocSize = newAllocSize;
        m_cacheBuff = newbuff;
    }

    memcpy(&m_cacheBuff[m_cachedSize], buffer, buffsize);
    m_cachedSize += buffsize;

next_package:
    if (m_cachedSize > 4)
    {
        if (m_haveGotSize == false)
        {
            m_pkgSize = (m_cacheBuff[0] << 24) | (m_cacheBuff[1] << 16) | (m_cacheBuff[2] << 8) | (m_cacheBuff[3]);
            m_haveGotSize = true;
        }

        if (m_haveGotSize)
        {
            if (m_cachedSize >= m_pkgSize)  // 积累够了一个数据包的缓冲大小
            {
                if (m_pkgSize)              
                {
                    VncDataPackage * package = new VncDataPackage(m_cacheBuff, m_pkgSize);
                    m_commandIn->addPackage(package);
                }
                m_haveGotSize = false;
                moveData();
                m_cachedSize -= m_pkgSize;
                if (m_cachedSize > 4)
                {
                    goto next_package;
                }
            }
        }
    }

    return buffsize;
}

void VncBufferReader::moveData()
{
    int moveByteSize, i;

    if (m_cachedSize <= m_pkgSize)
        return;
    moveByteSize = m_cachedSize - m_pkgSize;
    for (i = 0; i < moveByteSize; i++)
    {
        m_cacheBuff[i] = m_cacheBuff[m_pkgSize + i];
    }
}




