package com.coocaa.remotectrlservice;

public class GlobalData {

	private static GlobalData instance = null;
	private String mPushId = null;
	private String mTvId = null;
	private String mActiveId = null;
	private String mServerIp = null; 
	private String mDefaultServerIp = "223.202.11.125";//"120.27.147.96";
	private boolean mCurrentCtl = false;

	public static GlobalData getInstance() {
		if (instance == null) {
			instance = new GlobalData();
		}
		return instance;
	}

	private GlobalData() {

	}

	public void setPushId(String value) {
		mPushId = value;
	}

	public String getPushId() {
		return mPushId;
	}
	
	public void setTvId(String value) {
		mTvId = value;
	}

	public String getTvId() {
		return mTvId;
	}
	
	public void setActiveId(String value) {
		mActiveId = value;
	}
	
	public String getActiveId() {
		return mActiveId;
	}
	
	public void setServerIp(String value){
		mServerIp = value;
	}
	
	public String getServerIp(){
		return mServerIp;
	}
	
	public String getDefaultServerIp(){
		return mDefaultServerIp;
	}
	
	public void setCurrentControlStatus(boolean value){
		mCurrentCtl = value;
	}
	
	public boolean getCurrenControlStatus()
	{
		return mCurrentCtl;
	}
}
