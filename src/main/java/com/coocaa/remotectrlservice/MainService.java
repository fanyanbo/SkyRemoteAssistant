package com.coocaa.remotectrlservice;

import java.io.InputStream;
import java.lang.ref.WeakReference;
import java.net.HttpURLConnection;
import java.net.URL;

import org.json.JSONObject;

import com.coocaa.remotectrlservice.dialog.AuthorizedDialog;
import com.coocaa.remotectrlservice.dialog.StatusBarDialog;
import com.coocaa.remotectrlservice.dialog.TipsDialog;
import com.coocaa.remotectrlservice.R;
import com.skyworth.framework.skysdk.ipc.SkyService;
import com.skyworth.theme.SkyThemeEngine;
import com.tianci.system.api.TCSystemService;
import com.tianci.system.data.TCInfoSetData;
import com.tianci.system.define.TCEnvKey;

import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;

public class MainService extends SkyService {

	private static final String TAG = "RemoteCtrl";
	private static final String CMD_KEY = "cmd";
	private static final String CMD_CONNECT_REQ = "connect";
	private static final String CMD_CONNECT_SECRET = "secret";
	private static final String URL_GETIP = "http://tvagent.iot.coocaatv.com/getipaddr.php";
	public static MainService instance = null;
	public static TCSystemService mTcSystemService = null;
	private StatusBarDialog mStatusBarDlg = null;
	private AuthorizedDialog mAuthDlg = null;
	private TipsDialog mTipsDlg = null;
	private MyHandler mHandler = null;
	
	@Override
	public IBinder onBind(Intent arg0) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void onCreate() {
		// TODO Auto-generated method stub
		super.onCreate();
		
		Log.i(TAG, "tvagentservice->onCreate init jni");
		
		JniUtils.load();
		JniUtils.init(this, getPackageName());
		instance = this;
		
		Log.i(TAG, "tvagentservice->onCreate init ui");
		
		//SkyPush.registID(this, "2L1gbXK0", "NfBRXLwH");
		
		if (mHandler == null)
			mHandler = new MyHandler(this);
		if (mAuthDlg == null)
			mAuthDlg = AuthorizedDialog.getInstance(this);
		if (mStatusBarDlg == null)
			mStatusBarDlg = StatusBarDialog.getInstance(this);
		if (mTipsDlg == null)
			mTipsDlg = TipsDialog.getInstance(this);	
		
		new Thread(getThread).start();
	}
	
	private Thread getThread = new Thread(){
		 public void run() {
			 GlobalData.getInstance().setServerIp(getServerIp());
		 }
	};

	@Override
	public void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		// TODO Auto-generated method stub
		
		Log.i(TAG, "tvagentservice->onStartCommand start");
		if(intent != null && CMD_CONNECT_REQ.equals(intent.getStringExtra(CMD_KEY))){
			if(!GlobalData.getInstance().getCurrenControlStatus()){
				setTvId();
				mHandler.sendEmptyMessage(3);
			}
		}else if(intent != null && CMD_CONNECT_SECRET.equals(intent.getStringExtra(CMD_KEY))){
			setActiveId();
			mHandler.sendEmptyMessage(5);
		}
		Log.i(TAG, "tvagentservice->onStartCommand end");
		return START_NOT_STICKY;
		//return super.onStartCommand(intent, flags, startId);
	}
	
	private void showAuthorizedDialog()  
    {  		
		if(mAuthDlg != null){
			Log.i(TAG, "mAuthDlg != null , mAuthDlg.isShown() = " + mAuthDlg.isShown());
			if(!mAuthDlg.isShown()){
				if(GlobalData.getInstance().getCurrenControlStatus()){
					mAuthDlg.setTipContent(R.string.control_request_again);
				}else{
					mAuthDlg.setTipContent(R.string.control_request);
				}
				mAuthDlg.showDialog();
			}			
		}else{
			Log.i(TAG, "mAuthDlg == null");
			mAuthDlg = AuthorizedDialog.getInstance(this);
			mAuthDlg.showDialog();
		}
    }
	
	public String getVersion(){
		String ret = null;
		if(mTcSystemService != null){
			TCInfoSetData data = (TCInfoSetData)mTcSystemService.getSetData(TCEnvKey.SKY_SYSTEM_ENV_TIANCI_VER);
			if(data != null){
				ret = data.getCurrent();
				Log.i(TAG, "getVersion : " + ret);
			}
		}
		return ret;
	}
	
	public String getModel(){
		String ret = null;
		if(mTcSystemService != null){
			TCInfoSetData data = (TCInfoSetData)mTcSystemService.getSetData(TCEnvKey.SKY_SYSTEM_ENV_MODEL);
			if(data != null){
				ret = data.getCurrent();
				Log.i(TAG, "getModel : " + ret);
			}
		}
		return ret;
	}
	
	public String getType(){
		String ret = null;
		if(mTcSystemService != null){
			TCInfoSetData data = (TCInfoSetData)mTcSystemService.getSetData(TCEnvKey.SKY_SYSTEM_ENV_TYPE);
			if(data != null){
				ret = data.getCurrent();
				Log.i(TAG, "getType : " + ret);
			}
		}
		return ret;
	}
	
	public String getPanelSize(){
		String ret = null;
		if(mTcSystemService != null){
			TCInfoSetData data = (TCInfoSetData)mTcSystemService.getSetData(TCEnvKey.SKY_SYSTEM_ENV_PANEL_SIZE);
			if(data != null){
				ret = data.getCurrent();
				Log.i(TAG, "getPanelSize : " + ret);
			}
		}
		return ret;
	}
	
	public String getServerIp(){
		
		HttpURLConnection conn = null;
		try {
			URL url = new URL(URL_GETIP);
			conn = (HttpURLConnection) url.openConnection();
			conn.setConnectTimeout(3000);
            conn.setRequestMethod("GET");
            conn.setRequestProperty("Charset", "UTF-8");
            int code = conn.getResponseCode();
            if (code == 200) {
                InputStream is = conn.getInputStream();
                int resLen =0;
                byte[] res = new byte[1024];
                StringBuilder sb = new StringBuilder();
                while((resLen = is.read(res)) != -1){
                    sb.append(new String(res, 0, resLen));
                }
                String jsonStr = sb.toString();
                JSONObject obj = new JSONObject(jsonStr);
                String JsonDataStr = obj.get("data").toString();
                JSONObject dataObj = new JSONObject(JsonDataStr);
                String ip = dataObj.get("ipAddr").toString();
                Log.i(TAG,"getServerIp = " + ip);
                if(ip != null)
                	return ip;
                else 
                	return GlobalData.getInstance().getDefaultServerIp();
            } else {
            	Log.i(TAG,"getResponseCode != 200");
            }
		}catch(Exception e){
			Log.i(TAG,"getServerIp Exception " + e.getMessage());
			e.printStackTrace();
			return GlobalData.getInstance().getDefaultServerIp();
		} finally {		
			Log.i(TAG,"getServerIp finally");
			 if(conn != null){
				 conn.disconnect();
             }
		}
		Log.i(TAG,"getServerIp end");
		return GlobalData.getInstance().getDefaultServerIp();
	}
	
	
	public void dispatchMessage(int what)
	{
		Log.i(TAG, "dispatchMessage what = " + what);
		if(what == 3 || what == 4 || what == 5){
			if (mHandler == null)
				mHandler = new MyHandler(this);
			if (mStatusBarDlg == null)
				mStatusBarDlg = StatusBarDialog.getInstance(this);
			if (mAuthDlg == null)
				mAuthDlg = AuthorizedDialog.getInstance(this);
			if (mTipsDlg == null)
				mTipsDlg = TipsDialog.getInstance(this);	
		}

		if(mHandler != null){
			mHandler.sendEmptyMessage(what);
		}
	}
	
	 private static class MyHandler extends Handler{

	     private final WeakReference<MainService> mInstance;

	     public MyHandler(MainService view)
	     {
	    	// super(Looper.getMainLooper());
	    	 mInstance = new WeakReference<MainService>(view);
	     }
	        
		@Override
		public void handleMessage(Message msg) {
			
			Log.i(TAG, "MyHandler handleMessage msg.what = " + msg.what);
			MainService instance = mInstance.get();
			switch(msg.what)
			{
			case 0:
				break;
			case 1:
				if(!instance.mStatusBarDlg.isShown())
					instance.mStatusBarDlg.showDialog();
				break;
			case 2:
				if(instance.mStatusBarDlg.isShown())
				{
					instance.mStatusBarDlg.cancelDialog();
					instance.mTipsDlg.showDialog();
				}else{
					if(GlobalData.getInstance().getCurrenControlStatus())
						instance.mTipsDlg.showDialog();
				}
				break;
			case 3:
				instance.showAuthorizedDialog();
				break;
			case 4:
			{
				String ip = GlobalData.getInstance().getServerIp();
				String activeid = "SKYWORTHCOOCAA";
				Log.i(TAG, "TV accept control request ip:" + ip + ",activeid:" + activeid);
				if (activeid != null && ip != null) 
				{
					JniUtils.startVnc(activeid, ip, true);
				}
			}
				break;
			case 5:
			{
				String ip = GlobalData.getInstance().getServerIp();
				String activeid = GlobalData.getInstance().getActiveId();
				if(activeid == null){
					activeid = getActiveId();
				}
				activeid = activeid + "@";
				Log.i(TAG, "TV accept control request ip:" + ip + ",activeid:" + activeid);
				if (activeid != null && ip != null) 
				{
					JniUtils.startVnc(activeid, ip, true);
				}
			}
				break;
			case 6:
				instance.startUpgradeCheck();
				break;
			default:
				break;
			}
		}	
	};
	
	public void setPushId(String pushId)
	{
        GlobalData.getInstance().setPushId(pushId);

	}
	
	public void setTvId()
	{
		if(mTcSystemService != null){
			TCInfoSetData data = (TCInfoSetData)mTcSystemService.getSetData(TCEnvKey.SKY_SYSTEM_ENV_MACHINE_CODE);
			if(data != null)
				GlobalData.getInstance().setTvId(data.getCurrent());
		}
	}
	
	public void setActiveId()
	{
		if(mTcSystemService != null){
			TCInfoSetData data = (TCInfoSetData)mTcSystemService.getSetData(TCEnvKey.SKY_SYSTEM_ENV_ACTIVE_ID);
			if(data != null)
				GlobalData.getInstance().setActiveId(data.getCurrent());
		}
	}
	
	public static String getActiveId()
	{
		String ret = null;
		if(mTcSystemService != null){
			TCInfoSetData data = (TCInfoSetData)mTcSystemService.getSetData(TCEnvKey.SKY_SYSTEM_ENV_ACTIVE_ID);
			if(data != null){
				ret = data.getCurrent();
				Log.i(TAG, "getActiveId : " + ret);
			}
		}
		return ret;
	}
	
	public  void startUpgradeCheck()
	{
		Intent it = new Intent("android.settings.SYSTEM_UPGRADE");
		it.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		startActivity(it);
	}

	@Override
	public byte[] onHandler(String fromtarget, String cmd, byte[] body) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void onResult(String fromtarget, String cmd, byte[] body) {
		// TODO Auto-generated method stub
		
	}

	@Override
	@Deprecated
	public byte[] requestPause(String fromtarget, String cmd, byte[] body) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	@Deprecated
	public byte[] requestResume(String fromtarget, String cmd, byte[] body) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	@Deprecated
	public byte[] requestRelease(String fromtarget, String cmd, byte[] body) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	@Deprecated
	public byte[] requestStartToVisible(String fromtarget, String cmd,
			byte[] body) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	@Deprecated
	public byte[] requestStartToForground(String fromtarget, String cmd,
			byte[] body) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void onCmdConnectorInit() {
		// TODO Auto-generated method stub
		Log.i(TAG, "MainService,onCmdConnectorInit...");
		mTcSystemService = TCSystemService.getInstance(this);
	}
}
