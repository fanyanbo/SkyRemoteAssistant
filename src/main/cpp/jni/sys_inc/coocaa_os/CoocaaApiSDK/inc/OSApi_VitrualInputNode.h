#ifndef __OSAPI_VITRUAL_INPUT_NODE_H_
#define __OSAPI_VITRUAL_INPUT_NODE_H_
#include "CoocaaDef.h"

#include "OSApiListenerController.h"
#include "OSApi_Base.h"
NS_COOCAA_BEGIN

typedef enum {
	SENSOR_TYPE_ACCELEROMETER = 1,
	SENSOR_TYPE_MAGNETIC_FIELD = 2,
	SENSOR_TYPE_TYPE_GYROSCOPE = 4
} SENSOR_TYPE_t ;

class OSApi_VitrualInputNode:public OSApi_Base
{
	/*
	* 每一个api都会有多个返回值：
	*
	* 1：代表的是named pipe通信请求一切正常，并且处理结果返回成功的。此时不需要进行额外的处理。uiengine会自行处理界面切换。
	* 0：代表的是named pipe通信请求一切正常，但是由于上层程序的原因返回错误的结果，此时不需要额外操作，需要弹出一个提示框即可。
	* -1：代表的named pipe通信错误，可能是因为上层的壳程序崩溃。需要往另外的一个named pipe进行反馈。保障系统稳定性。
	*/
public:
	static const std::string SIMULATE_KEY_DOWN;
	static const std::string SIMULATE_KEY_UP;
	static const std::string SIMULATE_KEYCLICK;
	static const std::string SIMULATE_BUTTON_UP;
	static const std::string SIMULATE_BUTTON_DOWN;
	static const std::string SIMULATE_BUTTON_CLICK;
	static const std::string SIMULATE_SCROLL_UP;
	static const std::string SIMULATE_SCROLL_DOWN;

	static const std::string SIMULATE_MOUSE_MOVE;
	static const std::string SIMULATE_MOUSE_MOVETO;
	static const std::string SIMULATE_TOUCH;

	static const std::string SIMULATE_SENSOR_ACCELEROMETER;
	static const std::string SIMULATE_SENSOR_TYPE_MAGNETIC_FIELD;
	static const std::string SIMULATE_SENSOR_TYPE_TYPE_GYROSCOPE;

	OSApi_VitrualInputNode(OSApiListenerController* controller);
	virtual ~OSApi_VitrualInputNode();

	void simulate_key_down(int keyCode);
	void simulate_key_up(int keyCode);
	void simulate_key_click(int keyCode);

	void simulate_button_up(int keyCode);
	void simulate_button_down(int keyCode);
	void simulate_button_click(int keyCode);

	void simulate_scroll_up();
	void simulate_scroll_down();

	void simulate_mouse_move(float x_rel,float y_rel);
	void simulate_mouse_moveto(float x_abs,float y_abs);

	void simulate_touch(float x[],float y[],int status);
	//将参数发送到VirtualEvent
	void simulate_sensor(SENSOR_TYPE_t sensortype,long timestemp,float sensordata[],int size);

private:
	OSApi_VitrualInputNode(const OSApi_VitrualInputNode&);
	OSApi_VitrualInputNode& operator = (const OSApi_VitrualInputNode&);

	void simulateKeyCode(std::string forwhat,int keyCode);
};

NS_COOCAA_END


#endif //__OSAPI_VITRUAL_INPUT_NODE_H_