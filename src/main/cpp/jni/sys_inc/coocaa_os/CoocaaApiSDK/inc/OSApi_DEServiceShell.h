#ifndef __OSAPI_DESERVICE_SHELL_H_
#define __OSAPI_DESERVICE_SHELL_H_

#include "CoocaaDef.h"
#include <string>
#include "OSApiListenerController.h"
#include "OSApi_Base.h"
#include "ISocket.h"

NS_COOCAA_BEGIN
class OSApi_DEServiceShell:public OSApi_Base
{
public:

	OSApi_DEServiceShell(OSApiListenerController* controller);
	virtual ~OSApi_DEServiceShell();

	//notify started
	static const std::string NOTIFY_STARTED;

	static const std::string CMD_PLAYER_PLAY;
	static const std::string CMD_PLAYER_PAUSEORRESUME;
	static const std::string CMD_DONGLE_SWITCH_SOURCE;
	static const std::string CMD_TV_UPGRADE;
	static const std::string CMD_PLAYER_SEEK;
	static const std::string CMD_PLAYER_STOP;
	static const std::string CMD_PLAYER_OPENSEEKMODE;
	static const std::string CMD_PLAYER_CLOSESEEKMODE;
	static const std::string CMD_TV_SETVOLUME;
	static const std::string CMD_TV_MUTE;
	static const std::string CMD_TV_UNMUTE;
	static const std::string CMD_TV_MIRACASTMODE;
	static const std::string CMD_APNET_CONNECT;
	static const std::string CMD_APNET_DISCONNECT;
	static const std::string CMD_APNET_FORGET;
	static const std::string CMD_TEXT_VOICE;
	static const std::string CMD_TV_SETTING_CHANGE;
	static const std::string CMD_TV_RESET;//恢复出厂
	static const std::string CMD_USER_CLEARDATA;
	static const std::string CMD_PLAYER_START;
	static const std::string CMD_SENSOR_CHANGED;
	static const std::string CMD_APP_START;
	static const std::string CMD_APP_UNINSTALL;
	static const std::string CMD_APP_UPDATE;
	static const std::string CMD_UNKNOW;

	static const std::string INFO_GET_PLAYER_DATA;
	static const std::string INFO_GET_APNET_INFO;
	static const std::string INFO_GET_NETCONNECTED_INFO;
	static const std::string INFO_GET_NETSTATE;
	static const std::string INFO_GET_UPGRADE_DATA;
	static const std::string INFO_GET_APNET_LIST;
	static const std::string INFO_GET_APNET_STATELIST;
	static const std::string INFO_GET_SYSTEMSETTING_INFO;
	static const std::string INFO_GET_APP_INFO;
	static const std::string INFO_GET_DEVICE_MAC;
	static const std::string INFO_GET_DEVICE_INFO;
	static const std::string INFO_UNKNOW;

	static const std::string CHANNEL_READASYNC;
	static const std::string CHANNEL_READSYNC;

	static const std::string TARGET_MOBILE_ALIVE;
	static const std::string TARGET_MOBILE_DEAD;

    //begin airplay
    static const std::string AIRPLAY_STARTPLAYER;
    static const std::string AIRPLAY_STOPPLAYER;
    static const std::string AIRPLAY_SEEK;
    static const std::string AIRPLAY_SETVOLUME;
    static const std::string AIRPLAY_GET_INFO;
    static const std::string AIRPLAY_SETMUSICINFO;
    static const std::string AIRPLAY_SETCOVERART;
    //end airplay
	//init when dead
	void notifyStarted();

	bool cmd_play(std::string param);
	void cmd_pauseorresume();
	void cmd_dongleswitchsource(std::string param);
	void cmd_tv_upgrade(std::string param);
	void cmd_seek(std::string param);
	void cmd_stop();
	void cmd_openseekmode();
	void cmd_closeseekmode();
	void cmd_setvolume(std::string param);
	void cmd_setmute();
	void cmd_unmute();
	void cmd_setmiracastmode();
	void cmd_ap_connect(const SocketAddress&target,std::string param);
	void cmd_ap_disconnect(const SocketAddress& target);
	void cmd_ap_forget(std::string param);
	void cmd_text_input(std::string param);
	void cmd_changeSetting(std::string param);
	void cmd_resetSystem();
	void cmd_clear_userdata(std::string param);
	void cmd_callPlayer(std::string param);
	void cmd_sensor(std::string param);
	void cmd_startapp(std::string param);
	void cmd_uninstallapp(std::string param);
	void cmd_updataapp(std::string param);
	void cmd_unknow(std::string cmd, std::string param); // 增加未定义命令处理

	std::string info_getPlayInfo();
	std::string info_getApInfo(const SocketAddress&target);
	std::string info_getNetConnectinfo(const SocketAddress&target);
	std::string info_getNetConnectState();
	std::string info_getUpgradeInfo();
	std::string info_getApListInfo();
	std::string info_getApStateListInfo();
	std::string info_getSystemSetting();
	std::string info_getAppInfo();
	std::string info_getDeviceMac();
	std::string info_getSystemInfo();
	std::string info_getUnknow(std::string cmd); // 增加未定义命令处理

	void alive_remote(const SocketAddress&target);
	void dead_remote(const SocketAddress&target);

	std::string channel_readsync(const std::string& ipaddress,const std::string& port, const std::string& hostname,const std::string& jsonString);
	void channel_readasync(const std::string& ipaddress,const std::string& port, const std::string& hostname,const std::string& jsonString);

    //begin airplay
    bool airplay_startplayer(const std::string & url, const std::string url_mirror_audio, const std::string & type, long long pos);
    bool airplay_stopplayer(const std::string & type);
    bool airplay_seek(long long pos);
    bool airplay_setvolume(int value);
    std::string airplay_getInfo(const std::string & infotype);
    bool airplay_setMusicInfo(char* strSingerName, char* strSongName, char* strSpecialName);
    bool airplay_setCoverArt(std::string filename);
    //end airplay
private:
	OSApi_DEServiceShell(const OSApi_DEServiceShell&);
	OSApi_DEServiceShell& operator = (const OSApi_DEServiceShell&);
};

NS_COOCAA_END

#endif//__OSAPI_DESERVICE_SHELL_H_
