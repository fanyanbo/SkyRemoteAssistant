
#ifndef COOCAA_OS_VNC_COMMAND_PROCESSOR_H_
#define COOCAA_OS_VNC_COMMAND_PROCESSOR_H_

class VncDataPackage;

int processDataPackage(VncDataPackage * package);
int simpleFtpFile(int opt, const char * remoteFileName, const char * localFileName, int filetype, unsigned source, unsigned target, unsigned tag);

#define SIMFTP_OPT_PUT_FILE                 (0x00000000)
#define SIMFTP_OPT_GET_FILE                 (0x00000001)

#define SIMFTP_FILE_TYPE_COMMON             0
#define SIMFTP_FILE_TYPE_SCREEN             1
#define SIMFTP_FILE_TYPE_LOG                2

struct CatchPhotoInfo
{
    unsigned        m_tag;
    unsigned        m_second;
};

struct CatchLogInfo
{
    unsigned        m_tag;
    unsigned        m_second;
};

struct PrintLogInfo
{
	unsigned m_tag;
	std::string m_cmd;
};

struct PullFileInfo
{
	unsigned m_tag;
	std::string m_tvpath;
};

struct PushFileInfo
{
	unsigned    m_tag;
	std::string m_downloadUrl;
    std::string m_tvpath;
    unsigned    m_filesize;
};

#endif

