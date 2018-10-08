#ifndef __DataPacket
#define __DataPacket
#include "CoocaaDef.h"
#include <assert.h>
#include <string>
#include <atomic>  

NS_COOCAA_BEGIN

class DataPacket
{
public:
	DataPacket(const DataPacket& src)
	{
		_dataBuffer = src._dataBuffer;
		_dataBuffer->obtain();
	}
	DataPacket():_dataBuffer(nullptr)
	{
	}
	DataPacket(const unsigned int dataSize)
	{
		_dataBuffer = new DataBuffer(dataSize);
		assert(_dataBuffer!=nullptr);
	}
	DataPacket(const std::string& bufString)
	{
		_dataBuffer = new DataBuffer(bufString);
		assert(_dataBuffer != nullptr);
	}
	DataPacket(const unsigned char* srcbuffer,const unsigned int length)
	{
		_dataBuffer = new DataBuffer(srcbuffer,length);
		assert(_dataBuffer != nullptr);
	}

	virtual ~DataPacket()
	{
		if(_dataBuffer!=nullptr)
		{
			_dataBuffer->release();
		}
	}

	inline unsigned char* getBuffer()
	{
		return _dataBuffer->getBuffer();
	}

	inline unsigned getSize() const
	{
		return _dataBuffer->getBufferSize();
	}

	inline const unsigned char* getData() const
	{
		return _dataBuffer->getBuffer();
	}

	DataPacket & operator=(const DataPacket & src);
private:
	class DataBuffer
	{
	public:
		explicit DataBuffer()
			: _buffer(NULL),
			_size(0),
			_refCnt()
		{
		}
		explicit DataBuffer(const std::string& data)
			: _buffer(NULL),
			_size(0),
			_refCnt()
		{
			_buffer = reinterpret_cast<unsigned char*>(malloc(data.length()));
			if (_buffer != NULL)
			{
				_size = data.length();
			}
			memcpy(_buffer, data.c_str(), data.length());
			_refCnt = 1;
		}

		explicit DataBuffer(const unsigned char* data,const unsigned int length)
			: _buffer(NULL),
			_size(0),
			_refCnt()
		{
			_buffer = reinterpret_cast<unsigned char*>(malloc(length));
			if (_buffer != NULL)
			{
				_size = length;
			}
			memcpy(_buffer, data, length);
			_refCnt = 1;
		}

		explicit DataBuffer(const unsigned int bufferSize)
			: _buffer(NULL),
			_size(0),
			_refCnt()
		{

			_buffer = reinterpret_cast<unsigned char*>(calloc(bufferSize, sizeof(unsigned char)));
			if (_buffer != NULL)
			{
				_size = bufferSize;
			}
			_refCnt = 1;
		}
		virtual ~DataBuffer(){
		}

		unsigned char* getBuffer()
		{
			return _buffer;
		}
		unsigned int getBufferSize() const
		{
			return _size;
		}
		inline void release() 
		{
			assert(_refCnt > 0);
			if (--_refCnt == 0)         
			{
				if (_buffer != NULL)
				{
					free(_buffer);
					_buffer = NULL;
					_size = 0;
					delete this;
				}
			}
		}


		inline void obtain() 
		{ 
			assert(_refCnt > 0);
			++_refCnt;
		}
	private:
		unsigned char* _buffer;
		unsigned int _size;
		std::atomic<int> _refCnt;
	};


	DataBuffer* _dataBuffer;
};

NS_COOCAA_END

#endif//__DataPacket