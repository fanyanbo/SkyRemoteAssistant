#ifndef __OSAPI_AIRPLAY_BIN_H_
#define __OSAPI_AIRPLAY_BIN_H_
#include "CoocaaDef.h"

#include "OSApiListenerController.h"
#include "OSApi_Base.h"

NS_COOCAA_BEGIN
class OSApi_AirPlayService :public OSApi_Base
{
public:

    OSApi_AirPlayService(OSApiListenerController* controller);
    virtual ~OSApi_AirPlayService();

    void notifyStateChanged(std::string flag, std::string value);
    void notifyPlayerHaveExit();
    void notifyPlayerStatePlaying();
    void notifyPlayerStateBufferring();
    void notifyPlayerStateCurPos(int cur_pos);

    static std::string NOTIFY_STATE_CHANGED;
    static std::string PLAYER_HAVE_EXIT;
    static std::string PLAYER_STATE_PLAYING;
    static std::string PLAYER_STATE_BUFFERRING;
    static std::string PLAYER_STATE_CURRENTPOS;
};

NS_COOCAA_END

#endif//__OSAPI_AIRPLAY_BIN_H_
