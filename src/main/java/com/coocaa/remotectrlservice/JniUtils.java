
package com.coocaa.remotectrlservice;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import org.json.JSONException;
import org.json.JSONObject;

import com.coocaa.remotectrlservice.utils.ImageUtils;
import com.coocaa.remotectrlservice.utils.LogcatFileUtils;
import com.coocaa.remotectrlservice.utils.ShellUtils;
import com.coocaa.remotectrlservice.utils.ShellUtils.CommandResult;
import com.skyworth.framework.skysdk.logger.SkyLogger;
import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.util.Log;


public class JniUtils
{
	private static String TAG = "RemoteCtrl";
	private static Context jniContext = null;
	
	static 
	{
		try
        {
            System.loadLibrary("coocaavnc");
        } catch (Exception e)
        {
            Log.i(TAG, "loadLibrary coocaavnc exception = " + e.getMessage());
        }
	}
	
	public static void load()
    {
        // 空方法，只是为了触发static代码块中的loadLibrary
    }
	
	public static void init(Context pContext, String pkgName)
	{
		jniContext = pContext;
		initNativeProgram(pContext);
		nativeSetContext(pContext, pkgName);
	}
	
	public static void startVnc(String tvId, String ip, boolean accept)
	{
		String id;
		if (tvId == null)
			id = "xxxxxxx";
		else
			id = tvId;
		SkyLogger.i(TAG, "startVnc tvId :" + id + ", serverIp : " + ip);
		startCoocaaOsVnc(id, ip, accept);
	}
	
	private static void initNativeProgram(Context context)
	{
		/*
		Process process = null;
		DataOutputStream os = null;
		DataInputStream is = null;
		
		File file = new File("/system/vendor/bin/spawn");	// 如果coocaa os 5.0 自带的话
	    if(file.exists())
	    	return;
		
		String filesdir = context.getFilesDir().getAbsolutePath();
		
		SkyLogger.i(TAG, "filesdir = " + filesdir);
		
		copyAssetsDataToExeFile(context, "su", filesdir, true);
		copyAssetsDataToExeFile(context, "spawn", filesdir, false);
		copyAssetsDataToExeFile(context, "vnc", filesdir, false);
		
		try 
		{
			String suexe = filesdir + "/su";
			String vnc = filesdir + "/vnc";
			String spawn = filesdir + "/spawn";
			
            process = Runtime.getRuntime().exec("/system/bin/sh");   // exec(suexe);  // 执行suexe获得root
  
            os = new DataOutputStream(process.getOutputStream());
            is = new DataInputStream(process.getInputStream());
            os.writeBytes(spawn + " " + vnc + " \n");
            os.writeBytes("exit \n");  
            os.flush();  
            process.waitFor();
            
            SkyLogger.i(TAG, "exec ok.");
        } catch (Exception e) {    
        	SkyLogger.e(TAG, "exec fail.");
        	e.printStackTrace();
        } finally {  
            try {  
                if (os != null) {  
                    os.close();  
                }  
                if (is != null) {  
                    is.close();  
                }  
                process.destroy();  
            } catch (Exception e) {  
            }  
        }
        */
	}
	
	private static void copyAssetsDataToExeFile(Context context, String exeName, String filesDir, boolean suFlag)   
	{    
	    InputStream myInput = null;
	    OutputStream myOutput = null;
	    String outFileName = null;
	    File file = null;
	    
	    outFileName = filesDir + "/" + exeName;
	    
	    file = new File(outFileName);
	    if(!file.exists())    
	    {
	    	SkyLogger.i(TAG, "file " + outFileName + " not exist, create it.");
	    	
	    	try
		    {
	    		String cmd;
		    	Runtime runtime;
		    	
			    myOutput = new FileOutputStream(outFileName);
			    myInput = context.getAssets().open(exeName);    
			    byte[] buffer = new byte[1024];
			    int length = myInput.read(buffer);
			    while(length > 0)
			    {  
			        myOutput.write(buffer, 0, length);
			        length = myInput.read(buffer);
			    }  
			    myOutput.flush();
			    myInput.close();
			    myOutput.close();
			    
			    file = new File(outFileName);
			    if (suFlag)
			    {
			    	cmd = "chmod 4777 " + outFileName;						// 增加s权限
			    	runtime = Runtime.getRuntime();  
			    	runtime.exec(cmd);
			    	
			    	cmd = "chown root:root " + outFileName;					// 更改属主为root:root
			    	runtime = Runtime.getRuntime();  
			    	runtime.exec(cmd);
			    }
			    else
			    {
			    	cmd = "chmod 0755 " + outFileName;						// 增加x权限
			    	runtime = Runtime.getRuntime();  
			    	runtime.exec(cmd);
			    }
			    
			    SkyLogger.i(TAG, "copy " + outFileName + " ok.");
		    }
		    catch (IOException e)
		    {
		    	SkyLogger.e(TAG, "IO Error");
		    	e.printStackTrace();
		    }
	    }    
	    else
	    {
	    	SkyLogger.i(TAG, "file " + outFileName + " exist.");
	    }
	}

	private static String desPath = null;
	
	public static boolean execJCommand(String param1, String param2, String param3)
	{
		boolean ret = false;
		String cmd = param1;
		Log.i(TAG, "execJCommand param1=" + param1 + ",param2=" + param2 + ",param3=" + param3);
		if (cmd.equals("catch_screen"))
		{
//			handler.sendEmptyMessage(1);		
//			desPath = param2;
//			int count = 10;
//			while(true){
//				Log.i(TAG,"execJCommand catch_screen count = " + count + ",result.result = " + result.result );
//				if(result.result == 0){
//					break;
//				}
//				if(count == 0){
//					result = ShellUtils.execCommand("screencap -p /data/screen.png", false);
//					break;
//				}
//				try {
//					Thread.sleep(1000);
//				} catch (InterruptedException e) {
//					// TODO Auto-generated catch block
//					e.printStackTrace();
//				}
//				count--;
//			}

			try{
				CommandResult result = ShellUtils.execCommand("screencap -p /data/screen.png", false);
				SkyLogger.i(TAG, "result=="+ result.result);
				SkyLogger.i(TAG, "successMsg=="+ result.successMsg);
				SkyLogger.i(TAG, "errorMsg=="+ result.errorMsg);
				if (result.result == 0)
				{
					ImageUtils.compressBitmap(jniContext, "/data/screen.png", param2, 2, 85);
					ret = true;
				}
			}catch(Exception e){
				e.printStackTrace();
				SkyLogger.i(TAG,"error = " + e.toString());
				ret = false;
			}
		}
		/*
		else if (cmd.equals("upload_file"))
		{
			File file = new File(param2);
			try {
				new FtpClientUtils().uploadSingleFile(file, param3,
						new UploadProgressListener() {
							@Override
							public void onUploadProgress(
									String currentStep,
									long uploadSize, File file) {
								// TODO Auto-generated method
								Log.i(TAG, currentStep);
							}
						});
				ret = true;
			} catch (IOException e) {
				// TODO Auto-generated catch block
				Log.i(TAG, "ftp error");
				e.printStackTrace();
			}
		}
		*/
		else if (cmd.equals("logcat_start"))
		{
			LogcatFileUtils.getInstance().start(param2, param3, "logcat");
		}
		else if (cmd.equals("logcat_stop"))
		{
			LogcatFileUtils.getInstance().stop();
		}
		else if("logcat_print_start".equals(cmd))
		{
			//LogcatSocketUtils.getInstance().start();
		}
		else if("logcat_print_stop".equals(cmd))
		{
			//LogcatSocketUtils.getInstance().stop();
		}
		else if("logcat_print_param".equals(cmd))
		{
			//LogcatSocketUtils.getInstance().exec(param2);
		}
		else if("upgrade_check".equals(cmd))
		{
			if(MainService.instance != null){
				MainService.instance.dispatchMessage(6);
			}
		}
		else if("notify_pc_offline".equals(cmd))
		{
			if(MainService.instance != null){
				MainService.instance.dispatchMessage(2);
        		GlobalData.getInstance().setCurrentControlStatus(false);
			}
		}
		else if("control_success".equals(cmd))
		{
			if(MainService.instance != null){
        		MainService.instance.dispatchMessage(1);
        		GlobalData.getInstance().setCurrentControlStatus(true);
			}
		}
		else if("control_exit".equals(cmd))
		{
			if(MainService.instance != null){
        		MainService.instance.dispatchMessage(2);
        		GlobalData.getInstance().setCurrentControlStatus(false);
			}
		}

		return ret;
	}
	
	public static String execJCommandR(String param1, String param2, String param3)
	{
		Log.i(TAG, "execJCommandR param1=" + param1 + ",param2=" + param2 + ",param3=" + param3);
		String ret = "no result";
		String cmd = param1;
		if("GET_TV_BASEINFO".equals(cmd)){
			if(MainService.instance != null){
				String version = MainService.instance.getVersion();
				String model = MainService.instance.getModel();
				String type = MainService.instance.getType();
				String size = MainService.instance.getPanelSize();
				if(version == null || model == null || type == null || size == null)
				{
					ret = "GET_TV_BASEINFO data is null";
				}
				else
				{
					JSONObject jsonTvInfo = new JSONObject();
					try {
						jsonTvInfo.put("model", model);
						jsonTvInfo.put("version", version);
						jsonTvInfo.put("type", type);
						jsonTvInfo.put("size", size);
						ret = jsonTvInfo.toString();
					} catch (JSONException e) {
						// TODO Auto-generated catch block
						SkyLogger.i(TAG, "execJCommandR,GET_TV_BASEINFO,json error");
						e.printStackTrace();
						return "json error";
					}
					
				}
			}
		}
		return ret;
	}
	
	private static native void nativeSetContext(final Context pContext, String name);
	private static native void startCoocaaOsVnc(String pushId, String ip, boolean accept);
	
	private static Handler handler = new Handler() {

		@Override
		public void handleMessage(Message msg) {

			Log.i(TAG, "JniUtils handleMessage msg.arg1 = "+ msg.arg1 + ",msg.what = " + msg.what);
			switch (msg.what) 
			{
			case 0: 
				break;
			case 1: 
				CommandResult result = ShellUtils.execCommand("screencap -p /data/screen.png", false);
				SkyLogger.i(TAG, "result=="+ result.result);
				SkyLogger.i(TAG, "successMsg=="+ result.successMsg);
				SkyLogger.i(TAG, "errorMsg=="+ result.errorMsg);
				if (result.result == 0)
					ImageUtils.compressBitmap(jniContext, "/data/screen.png", desPath, 2, 85);
				break;
			default:
				break;
			}
		}

	};
}





