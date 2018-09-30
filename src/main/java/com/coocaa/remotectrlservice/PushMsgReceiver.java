package com.coocaa.remotectrlservice;

import com.skyworth.framework.skysdk.push.SkyPush;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class PushMsgReceiver extends BroadcastReceiver {

	private String TAG = "RemoteCtrl";
	@Override
	public void onReceive(Context context, Intent intent) {
		// TODO Auto-generated method stub
        if (intent.getAction().equals(SkyPush.BC_COOCAAPUSH_MSG))
        {
            String msg = intent.getStringExtra(SkyPush.MSG_RESULT_KEY);
            Log.i(TAG, "PushMsgReceiver->msg======>"+msg);
//            if("connect".equals(msg) && !GlobalData.getInstance().getCurrenControlStatus()){
//				if(MainService.instance != null){
//					MainService.instance.setTvId();
//					MainService.instance.setActiveId();
//            		MainService.instance.dispatchMessage(3);
//            	}else{
//            		Log.i(TAG, "MainService.instance == null");
//            	}
//            }   
        }
        else if (intent.getAction().equals(SkyPush.BC_COOCAAPUSH_REGISTER))
        {
            int result = intent.getIntExtra(SkyPush.REGID_RESULT_KEY, 0);
            String rid = intent.getStringExtra(SkyPush.REGID_RESULT_REGID_KEY);
            Log.i(TAG, "PushMsgReceiver->result ==="+result+",rid==="+rid);
//        	if(MainService.instance != null){
//        		MainService.instance.setPushId(rid);
//        		MainService.instance.setTvId();
//        		MainService.instance.setActiveId();
//        	}
        }
        else if(intent.getAction().equals("com.coocaa.get.id"))
        {
//        	Log.i(TAG, "PushMsgReceiver->getPushid======" + GlobalData.getInstance().getPushId());
//        	Log.i(TAG, "PushMsgReceiver->getActiveId======" + GlobalData.getInstance().getActiveId());
        	Log.i(TAG, "PushMsgReceiver->getTvId======" + GlobalData.getInstance().getTvId());
        }
        else if(intent.getAction().equals("com.coocaa.test.auth"))
        {
        	if(MainService.instance != null){
        		MainService.instance.dispatchMessage(3);
        	}
        }
        else if(intent.getAction().equals("com.coocaa.initiative.request"))
        {
            if(!GlobalData.getInstance().getCurrenControlStatus()){
				if(MainService.instance != null){
            		MainService.instance.dispatchMessage(4);
            	}else{
            		Log.i(TAG, "MainService.instance == null");
            	}
            } 
        }
        else if(intent.getAction().equals("com.coocaa.initiative.request2")){
            if(!GlobalData.getInstance().getCurrenControlStatus()){
				if(MainService.instance != null){
            		MainService.instance.dispatchMessage(5);
            	}else{
            		Log.i(TAG, "MainService.instance == null");
            	}
            } 
        }
	}

}
