package com.coocaa.remotectrlservice.utils;
//package com.coocaa.tvagentservice.utils;
//
//import java.io.BufferedReader;
//import java.io.BufferedWriter;
//import java.io.IOException;
//import java.io.InputStreamReader;
//import java.io.OutputStreamWriter;
//import java.net.Socket;
//import java.net.UnknownHostException;
//
//import android.util.Log;
//
//public class LogcatSocketUtils {
//
//	private static LogcatSocketUtils INSTANCE = null;
//	private LogDumper mLogDumper = null;
//
//	public static LogcatSocketUtils getInstance() {
//		if (INSTANCE == null) {
//			INSTANCE = new LogcatSocketUtils();
//		}
//		return INSTANCE;
//	}
//
//	private LogcatSocketUtils() {
//	}
//
//
//	public void start() {
//		if (mLogDumper == null) {
//			mLogDumper = new LogDumper();
//		}
//		
//	}
//	
//	public void start(String cmd) {
//		if (mLogDumper == null) {
//			mLogDumper = new LogDumper(cmd);
//			mLogDumper.start();
//		}
//		
//	}
//
//	public void stop() {
//		if (mLogDumper != null) {
//			mLogDumper.clear();
//			mLogDumper = null;
//		}
//	}
//	
//	public void exec(String cmd){
//		if (mLogDumper == null) {
//			mLogDumper = new LogDumper(cmd);
//			mLogDumper.start();
//		}else{
//			mLogDumper.reset(cmd);
//			try {
//				Thread.sleep(50);
//			} catch (InterruptedException e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//			}
//			mLogDumper.start();
////			if(!"pause".equals(cmd))
////			{
////				mLogDumper.start();
////			}else{
////				
////			}
//		}
//	}
//
//	private class LogDumper extends Thread {
//		private Process logcatProc = null;
//		private BufferedReader mReader = null;
//		private BufferedWriter mWriter = null;
//		private boolean mRunning = true;
//		private String cmds = null;
//		private Socket socket = null;  
//		private char[] buf = null;
//		
//		public LogDumper() {
//			Log.i("fyb","LogDumper()");
//			try {
//				socket = new Socket("172.20.115.104", 8004);
//			} catch (UnknownHostException e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//			} catch (IOException e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//				Log.i("fyb",e.getMessage());
//			} 
//		}
//		
//		public LogDumper(String cmd) {
//			Log.i("fyb","LogDumper(String cmd)");
//			try {
//				socket = new Socket("172.20.115.104", 8004);
//			} catch (UnknownHostException e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//			} catch (IOException e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//				Log.i("fyb",e.getMessage());
//			} 
//			cmds = cmd;
//		}
//
//		public void clear() {
//			Log.i("fyb","clear()");
//			mRunning = false;
//			if (logcatProc != null) {
//				logcatProc.destroy();
//				logcatProc = null;
//			}
//			if (mReader != null) {
//				try {
//					mReader.close();
//					mReader = null;
//				} catch (IOException e) {
//					e.printStackTrace();
//				}
//			}
//			if (mWriter != null) {
//				try {
//					mWriter.close();
//				} catch (IOException e) {
//					e.printStackTrace();
//				}
//				mWriter = null;
//			}
//			if(socket != null){
//				try {
//					socket.close();
//				} catch (IOException e) {
//					e.printStackTrace();
//				}
//				socket = null;
//			}
//		}
//		
//		public void reset(String cmd){
//			Log.i("fyb","reset cmd " + cmd);
//			mRunning = false;
//			if (logcatProc != null) {
//				logcatProc.destroy();
//				logcatProc = null;
//			}
//			if (mReader != null) {
//				try {
//					mReader.close();
//					mReader = null;
//				} catch (IOException e) {
//					e.printStackTrace();
//				}
//			}
//			if (mWriter != null) {
//				try {
//					mWriter.close();
//				} catch (IOException e) {
//					e.printStackTrace();
//				}
//				mWriter = null;
//			}
//			cmds = cmd;
//			mRunning = true;
//		}
//
//		@Override
//		public void run() {
//			Log.i("fyb","run()........................");
//			try {	 
//				mWriter = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));
//				logcatProc = Runtime.getRuntime().exec(cmds);
//				mReader = new BufferedReader(new InputStreamReader(
//						logcatProc.getInputStream()),8192);
//				int bufferSize = 8192;
//				buf = new char[bufferSize];
//				int read = 0;
//				while (mRunning && (read = mReader.read(buf,0,buf.length)) != -1) {
//					if (!mRunning) {
//						break;
//					}
//					Log.i("fyb","run() buf = " + buf);
//					Log.i("fyb","run() read n = " + read);
//					if (mWriter != null) {
//						mWriter.write(buf, 0, read);
//						mWriter.flush(); 
//					}
//				}
//				mWriter.flush(); 
//			} catch (IOException e) {
//				e.printStackTrace();
//				Log.i("fyb","run error : " + e.getMessage());
//			} finally {
//				if (logcatProc != null) {
//					logcatProc.destroy();
//					logcatProc = null;
//				}
//				if (mReader != null) {
//					try {
//						mReader.close();
//						mReader = null;
//					} catch (IOException e) {
//						e.printStackTrace();
//					}
//				}
//				if (mWriter != null) {
//					try {
//						mWriter.close();
//					} catch (IOException e) {
//						e.printStackTrace();
//					}
//					mWriter = null;
//				}
//			}
//		}
//	}
//}
