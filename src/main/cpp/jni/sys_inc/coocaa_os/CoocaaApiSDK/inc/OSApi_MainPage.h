#ifndef __OSAPI_MAINPAGE_API_H
#define __OSAPI_MAINPAGE_API_H
#include "CoocaaDef.h"
#include <string>
#include <thread>
#include "OSApiListenerController.h"
#include "OSApi_Base.h"

NS_COOCAA_BEGIN

class OSApi_MainPage:public OSApi_Base
{
public:

	//Ö÷Ò³ÃüÁî
	static const std::string SHOW_MAINPAGE_CMD;
	static const std::string HIDE_MAINPAGE_CMD;
	static const std::string NITIFY_EVENT_CHANGED;
	static const std::string REQUEST_FOCUS;
	static const std::string DROP_FOCUS;
	static const std::string INPUT_KEY;

	OSApi_MainPage(OSApiListenerController* controller);
	virtual ~OSApi_MainPage();
	/*
	* Í¬²½api
	*/
	bool showMainPage();

	bool hideMainPage();

	bool notifyEventChanged();
	
	bool requestFocus(bool focused);

	void inputKeyCode(int keyCode);
private:
	OSApi_MainPage(const OSApi_MainPage&);
	OSApi_MainPage& operator = (const OSApi_MainPage&);
};

NS_COOCAA_END

#endif //__OSAPI_MAINPAGE_API_5_0_H
