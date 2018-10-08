#ifndef __APISTRING_UTIL_H_
#define __APISTRING_UTIL_H_

#include <string>
#include <vector>
#include <stdlib.h>
#include <ctype.h>

inline int SplitString(const std::string &srcStr,const std::string &splitStr,std::vector<std::string> &destVec)
{
	if(srcStr.size()==0)
	{   
		return 0;
	}   
	size_t oldPos,newPos;
	oldPos=0;
	newPos=0;
	std::string tempData;
	while(1)
	{   
		newPos=srcStr.find(splitStr,oldPos);
		if(newPos!=std::string::npos)
		{   
			tempData = srcStr.substr(oldPos,newPos-oldPos);
			destVec.push_back(tempData);
			oldPos=newPos+splitStr.size();
		}   
		else if(oldPos<=srcStr.size())
		{   
			tempData= srcStr.substr(oldPos);
			destVec.push_back(tempData);
			break;
		}   
		else
		{   
			break;
		}   
	}   
	return 0;
}

inline std::string Int2String(int n) {

	char t[16];
#ifdef _WIN32
	sprintf_s(t, 16,"%d", n);
#elif ANDROID
	sprintf(t, "%d", n);
#endif

	return std::string(t);
}

inline std::string Float2String(float n) {

	char t[16];
#ifdef _WIN32
	sprintf_s(t, 16,"%f", n);
#elif ANDROID
	sprintf(t, "%f", n);
#endif

	return std::string(t);
}

inline std::string Long2String(long n) {

	char t[16];
#ifdef _WIN32
	sprintf_s(t, 16,"%ld", n);
#elif ANDROID
	sprintf(t, "%ld", n);
#endif

	return std::string(t);
}

inline int String2Int(std::string n) {
	int i = 0;
	i = atoi(n.c_str());
	return i;
}

inline float String2Float(std::string n) {
	double i = 0;
	i = atof(n.c_str());
	return (float)i;
}

inline long String2Long(std::string n) {
	long i = 0;
	i = atol(n.c_str());
	return i;
}

inline std::string GetTcVersion()
{
	char ch[128];
	FILE* file;

#ifdef _WIN32
	fopen_s(&file,"/system/vendor/TianciVersion","rt");
#elif ANDROID
	file = fopen("/system/vendor/TianciVersion","rt");
#endif

	if(file== NULL)
	{
		return "";
	}
	std::string version = "" ;
	while(1)
	{
		if (fgets(ch,sizeof(ch),file) == NULL)
		{
			break;
		}
		else
		{
			char delim[2] = ".";
			char *p = NULL, *pNext = NULL;
#ifdef _WIN32
			p = strtok_s( ch, delim,&pNext);
#elif ANDROID
			p = strtok(ch,delim);
#endif
			int i = 0;
			while( p != NULL ) {
				if(i == 1)
				{ //if second version code length ==1, version must be add 0~
					if(strlen(p) == 1)
					{
						std::string realp = std::string("0") +std::string(p);
						version += realp;
					}else
					{
						version += std::string(p);
					}
				}
				else if(i==2)
				{//if third verison code ==6,version ok~
					if(strlen(p) < 6)
					{
						std::string prefix = std::string("");
						for(int i=strlen(p);i<6;i++)
						{
							prefix +="0";
						}
						std::string realp = prefix +std::string(p);
						version += realp;
					}
					else
					{
						version += std::string(p);
					}
				}
				else
				{
					version +=  std::string(p);
				}
#ifdef _WIN32
				p = strtok_s( NULL, delim,&pNext);
#elif ANDROID
				p = strtok( NULL, delim );
#endif
				i ++;
			}

			break;
		}

	}
	fclose(file);
	return version;
}

static const std::string base64_chars =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";

static inline bool is_base64(unsigned char c)
{
	return (isalnum(c) || (c == '+') || (c == '/'));
}

inline unsigned int base64_encode(const unsigned char* bytes_to_encode, unsigned int in_len, unsigned char* encoded_buffer, unsigned int& out_len)
{
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3] = { 0, 0, 0 };
	unsigned char char_array_4[4] = { 0, 0, 0, 0 };

	out_len = 0;
	while (in_len--)
	{
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3)
		{
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; i < 4 ; i++)
			{
				encoded_buffer[out_len++] = base64_chars[char_array_4[i]];
			}
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
		{
			char_array_3[j] = '\0';
		}

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; j < (i + 1); j++)
		{
			encoded_buffer[out_len++] = base64_chars[char_array_4[j]];
		}

		while (i++ < 3)
		{
			encoded_buffer[out_len++] = '=';
		}
	}

	return out_len;
}

inline unsigned int base64_decode(const unsigned char* encoded_string, unsigned int in_len, unsigned char* decoded_buffer, unsigned int& out_len)
{
	size_t i = 0;
	size_t j = 0;
	int in_ = 0;
	unsigned char char_array_3[3] = { 0, 0, 0 };
	unsigned char char_array_4[4] = { 0, 0, 0, 0 };

	out_len = 0;
	while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
	{
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i == 4)
		{
			for (i = 0; i < 4; i++)
			{
				char_array_4[i] = static_cast<unsigned char>(base64_chars.find(char_array_4[i]));
			}

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; i < 3; i++)
			{
				decoded_buffer[out_len++] = char_array_3[i];
			}
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 4; j++)
		{
			char_array_4[j] = 0;
		}

		for (j = 0; j < 4; j++)
		{
			char_array_4[j] = static_cast<unsigned char>(base64_chars.find(char_array_4[j]));
		}

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++)
		{
			decoded_buffer[out_len++] = char_array_3[j];
		}
	}
	return out_len;
}

#endif //__APISTRING_UTIL_H_