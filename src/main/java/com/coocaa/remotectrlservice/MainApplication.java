package com.coocaa.remotectrlservice;

import android.util.Log;

import com.skyworth.framework.skysdk.ipc.SkyApplication;
import com.skyworth.theme.SkyThemeEngine;

public class MainApplication extends SkyApplication{

	@Override
	public void onCreate() {
		// TODO Auto-generated method stub
		super.onCreate();
		
		initTheme();
	}
	
	private void initTheme() {
		 Log.i("RemoteCtrl","MainApplication, initTheme()");
	     SkyThemeEngine.getInstance().init(this);
	}
}
