

#ifndef COOCAA_OS_VNC_BUFFER_READER_H_
#define COOCAA_OS_VNC_BUFFER_READER_H_

class VncCommandQueue;

class VncBufferReader
{
public:
    VncBufferReader(VncCommandQueue * cmdIn);
    ~VncBufferReader();

    int addBuffer(unsigned char * buffer, unsigned buffsize);

private:
    void freeCacheBuffer();
    void moveData();

    VncCommandQueue *   m_commandIn;
    unsigned int        m_allocSize;
    unsigned int        m_cachedSize;
    unsigned char *     m_cacheBuff;
    unsigned int        m_pkgSize;
    bool                m_haveGotSize;
};



#endif





