
#include "VirtualKey.h"
#include "IPCMessage.h"
#include "OSApiListenerController.h"
#include "OSApi_VitrualInputNode.h"
#include "VncUtils.h"
#include "VncCtrl.h"

coocaa::OSApiListenerController* controller = nullptr;

static coocaa::IPCMessage process(const coocaa::IPCMessage& srcmsg, bool& canProcess)
{
    coocaa::IPCMessage recMsg = srcmsg;

    if (recMsg.isEmpty())
    {
        canProcess = false;
        return coocaa::IPCMessage();
    }

    canProcess = true;

    return coocaa::IPCMessage();
}


int virtual_key_init()
{
    if (controller == nullptr)
    {
        controller = coocaa::OSApiListenerController::getInstance("packagename");
        controller->registerProcessor(&process);
    }
    


    //OSApi_DEServiceBin osapi(controller);
    //osapi.startUp(devicename, version, devicetype);
}

int embVirtualKeyClick(unsigned code)
{
    coocaa::OSApi_VitrualInputNode inputNode(controller);
    unsigned keyValue = 0x80000000 + code;
    VNCLOGD("write keyValue = 0x%08x", keyValue);
    inputNode.simulate_key_click(keyValue);
}





