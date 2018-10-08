#include "com_coocaa_remotectrlservice_JniUtils.h"
#include <string>
#include <stdio.h>
#include "JniHelper.h"
#include "VncCtrl.h"
#include "KeyDispatchManager.h"

using namespace vnc;

#define CLASS_NAME "com/coocaa/remotectrlservice/JniUtils"


#ifdef __cplusplus
extern "C" {
#endif

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	JniHelper::setJavaVM(vm);

	return JNI_VERSION_1_4;
}

pthread_t thread_handle = -1;
static void * vnc_test_thread(void * args);

JNIEXPORT void JNICALL Java_com_coocaa_remotectrlservice_JniUtils_nativeSetContext
		(JNIEnv* env, jclass thiz, jobject context, jstring pkgname)
{
	JniHelper::setClassLoaderFrom(context);
	const char* pszText = env->GetStringUTFChars(pkgname, nullptr);

	//std::string packagename(pszText,strlen(pszText));

	env->ReleaseStringUTFChars(pkgname, pszText);

	KeyDispatchManager::getInstance()->init(); 

    return;
}

JNIEXPORT jboolean JNICALL Java_com_coocaa_remotectrlservice_JniUtils_startCoocaaOsVnc
        (JNIEnv* env, jclass thiz, jstring pushID, jstring serverIp, jboolean accept)
{
    jboolean ret;
    bool cAccept;
    const char* pszText = env->GetStringUTFChars(pushID, nullptr);
    std::string pushIdString(pszText, strlen(pszText));

	const char* pServerIp = env->GetStringUTFChars(serverIp, nullptr);
	std::string serverIpString(pServerIp, strlen(pServerIp));

    if (accept == JNI_TRUE)
        cAccept = true;
    else
        cAccept = false;

	if (startVnc(pushIdString.c_str(), serverIpString, cAccept) == 0)
        ret = JNI_TRUE;
    else
        ret = JNI_FALSE;

    env->ReleaseStringUTFChars(pushID, pszText);
	env->ReleaseStringUTFChars(serverIp, pServerIp);

    return ret;
}

bool execJCommand(std::string cmd, std::string param2, std::string param3)
{
    JniMethodInfo t;

    bool ret = false;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "execJCommand", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z")) 
    {
        jstring stringArg1 = t.env->NewStringUTF(cmd.c_str());
        jstring stringArg2 = t.env->NewStringUTF(param2.c_str());
        jstring stringArg3 = t.env->NewStringUTF(param3.c_str());
        ret = t.env->CallStaticBooleanMethod(t.classID, t.methodID, stringArg1, stringArg2, stringArg3);
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg1);
        t.env->DeleteLocalRef(stringArg2);
        t.env->DeleteLocalRef(stringArg3);
    }
    return ret;
}

std::string execJCommandR(std::string cmd, std::string param2, std::string param3)
{
    JniMethodInfo t;

    std::string ret("");
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "execJCommandR", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;"))
    {
        jstring stringArg1 = t.env->NewStringUTF(cmd.c_str());
        jstring stringArg2 = t.env->NewStringUTF(param2.c_str());
        jstring stringArg3 = t.env->NewStringUTF(param3.c_str());
        jstring str = (jstring) t.env->CallStaticObjectMethod(t.classID, t.methodID, stringArg1, stringArg2, stringArg3);
        ret = JniHelper::jstring2string(str);
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg1);
        t.env->DeleteLocalRef(stringArg2);
        t.env->DeleteLocalRef(stringArg3);
        t.env->DeleteLocalRef(str);
    }
    return ret;
}

#ifdef __cplusplus
}
#endif

