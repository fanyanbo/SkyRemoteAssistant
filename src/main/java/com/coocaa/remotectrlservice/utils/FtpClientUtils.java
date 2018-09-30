package com.coocaa.remotectrlservice.utils;
//package com.coocaa.tvagentservice.utils;
//
//import java.io.File;
//import java.io.FileInputStream;
//import java.io.FileOutputStream;
//import java.io.IOException;
//import java.io.InputStream;
//import java.io.OutputStream;
//import java.util.LinkedList;
//
//import org.apache.commons.net.ftp.FTPClient;
//import org.apache.commons.net.ftp.FTPClientConfig;
//import org.apache.commons.net.ftp.FTPFile;
//import org.apache.commons.net.ftp.FTPReply;
//
//import com.coocaa.tvagentservice.MainActivity;
//
//
//public class FtpClientUtils {
//
//	private String hostName = null;
//	private int serverPort = 21;
//	private String userName = null;
//	private String password = null;
//	private FTPClient ftpClient = null;
//
//	public FtpClientUtils() {
//		this.hostName = "172.20.115.136";
//		this.serverPort = 21;
//		this.userName = "skyserver";
//		this.password = "000000";
//		this.ftpClient = new FTPClient();
//	}
//	
//	/**
//	 * 上传单个文件.
//	 * 
//	 * @param localFile
//	 *            本地文件
//	 * @param remotePath
//	 *            FTP目录
//	 * @param listener
//	 *            监听器
//	 * @throws IOException
//	 */
//	public void uploadSingleFile(File singleFile, String remotePath,
//			UploadProgressListener listener) throws IOException {
//
//
//		this.uploadBeforeOperate(remotePath, listener);
//
//		boolean flag;
//		flag = uploadingSingle(singleFile, listener);
//		if (flag) {
//			listener.onUploadProgress(MainActivity.FTP_UPLOAD_SUCCESS, 0,
//					singleFile);
//		} else {
//			listener.onUploadProgress(MainActivity.FTP_UPLOAD_FAIL, 0,
//					singleFile);
//		}
//
//		this.uploadAfterOperate(listener);
//	}
//	
//	/**
//	 * 上传多个文件.
//	 * 
//	 * @param localFile
//	 *            本地文件
//	 * @param remotePath
//	 *            FTP目录
//	 * @param listener
//	 *            监听器
//	 * @throws IOException
//	 */
//	public void uploadMultiFile(LinkedList<File> fileList, String remotePath,
//			UploadProgressListener listener) throws IOException {
//
//		this.uploadBeforeOperate(remotePath, listener);
//
//		boolean flag;
//
//		for (File singleFile : fileList) {
//			flag = uploadingSingle(singleFile, listener);
//			if (flag) {
//				listener.onUploadProgress(MainActivity.FTP_UPLOAD_SUCCESS, 0,
//						singleFile);
//			} else {
//				listener.onUploadProgress(MainActivity.FTP_UPLOAD_FAIL, 0,
//						singleFile);
//			}
//		}
//
//		this.uploadAfterOperate(listener);
//	}
//
//	private boolean uploadingSingle(File localFile,
//			UploadProgressListener listener) throws IOException {
//		boolean flag = true;
//		// 不带进度的方式
//		 // 创建输入流
//		 InputStream inputStream = new FileInputStream(localFile);
//		 // 上传单个文件
//		 flag = ftpClient.storeFile(localFile.getName(), inputStream);
//		 // 关闭文件流
//		 inputStream.close();
//		 
//
//		// 带有进度的方式
//		//BufferedInputStream buffIn = new BufferedInputStream(
//		//		new FileInputStream(localFile));
//		//ProgressInputStream progressInput = new ProgressInputStream(buffIn,
//		//		listener, localFile);
//		//flag = ftpClient.storeFile(localFile.getName(), progressInput);
//		//buffIn.close();
//
//		return flag;
//	}
//	
//	/**
//	 * 上传文件之前初始化相关参数
//	 * 
//	 * @param remotePath
//	 *            FTP目录
//	 * @param listener
//	 *            监听器
//	 * @throws IOException
//	 */
//	private void uploadBeforeOperate(String remotePath,
//			UploadProgressListener listener) throws IOException {
//
//		try {
//			this.openConnect();
//			listener.onUploadProgress(MainActivity.FTP_CONNECT_SUCCESSS, 0,
//					null);
//		} catch (IOException e1) {
//			e1.printStackTrace();
//			listener.onUploadProgress(MainActivity.FTP_CONNECT_FAIL, 0, null);
//			return;
//		}
//		
//		ftpClient.setFileTransferMode(org.apache.commons.net.ftp.FTP.STREAM_TRANSFER_MODE);
//		ftpClient.makeDirectory(remotePath);
//		ftpClient.changeWorkingDirectory(remotePath);
//	}
//
//	/**
//	 * 上传完成之后关闭连接
//	 * 
//	 * @param listener
//	 * @throws IOException
//	 */
//	private void uploadAfterOperate(UploadProgressListener listener)
//			throws IOException {
//		this.closeConnect();
//		listener.onUploadProgress(MainActivity.FTP_DISCONNECT_SUCCESS, 0, null);
//	}
//	
//	/**
//	 * 下载单个文件，可实现断点下载.
//	 * 
//	 * @param serverPath
//	 *            Ftp目录及文件路径
//	 * @param localPath
//	 *            本地目录
//	 * @param fileName       
//	 *            下载之后的文件名称
//	 * @param listener
//	 *            监听器
//	 * @throws IOException
//	 */
//	public void downloadSingleFile(String serverPath, String localPath, String fileName, DownLoadProgressListener listener)
//			throws Exception {
//
//		try {
//			this.openConnect();
//			listener.onDownLoadProgress(MainActivity.FTP_CONNECT_SUCCESSS, 0, null);
//		} catch (IOException e1) {
//			e1.printStackTrace();
//			listener.onDownLoadProgress(MainActivity.FTP_CONNECT_FAIL, 0, null);
//			return;
//		}
//
//		FTPFile[] files = ftpClient.listFiles(serverPath);
//		if (files.length == 0) {
//			listener.onDownLoadProgress(MainActivity.FTP_FILE_NOTEXISTS, 0, null);
//			return;
//		}
//
//		File mkFile = new File(localPath);
//		if (!mkFile.exists()) {
//			mkFile.mkdirs();
//		}
//
//		localPath = localPath + fileName;
//		// 接着判断下载的文件是否能断点下载
//		long serverSize = files[0].getSize(); // 获取远程文件的长度
//		File localFile = new File(localPath);
//		long localSize = 0;
//		if (localFile.exists()) {
//			localSize = localFile.length(); // 如果本地文件存在，获取本地文件的长度
//			if (localSize >= serverSize) {
//				File file = new File(localPath);
//				file.delete();
//			}
//		}
//		
//		long step = serverSize / 100;
//		long process = 0;
//		long currentSize = 0;
//		OutputStream out = new FileOutputStream(localFile, true);
//		ftpClient.setRestartOffset(localSize);
//		InputStream input = ftpClient.retrieveFileStream(serverPath);
//		byte[] b = new byte[1024];
//		int length = 0;
//		while ((length = input.read(b)) != -1) {
//			out.write(b, 0, length);
//			currentSize = currentSize + length;
//			if (currentSize / step != process) {
//				process = currentSize / step;
//				if (process % 5 == 0) {  
//					listener.onDownLoadProgress(MainActivity.FTP_DOWN_LOADING, process, null);
//				}
//			}
//		}
//		out.flush();
//		out.close();
//		input.close();
//		
//		// 此方法是来确保流处理完毕，如果没有此方法，可能会造成现程序死掉
//		if (ftpClient.completePendingCommand()) {
//			listener.onDownLoadProgress(MainActivity.FTP_DOWN_SUCCESS, 0, new File(localPath));
//		} else {
//			listener.onDownLoadProgress(MainActivity.FTP_DOWN_FAIL, 0, null);
//		}
//
//		this.closeConnect();
//		listener.onDownLoadProgress(MainActivity.FTP_DISCONNECT_SUCCESS, 0, null);
//
//		return;
//	}
//
//	/**
//	 * 删除Ftp下的文件.
//	 * 
//	 * @param serverPath
//	 *            Ftp目录及文件路径
//	 * @param listener
//	 *            监听器
//	 * @throws IOException
//	 */
//	public void deleteSingleFile(String serverPath, DeleteFileProgressListener listener)
//			throws Exception {
//
//		try {
//			this.openConnect();
//			listener.onDeleteProgress(MainActivity.FTP_CONNECT_SUCCESSS);
//		} catch (IOException e1) {
//			e1.printStackTrace();
//			listener.onDeleteProgress(MainActivity.FTP_CONNECT_FAIL);
//			return;
//		}
//
//		FTPFile[] files = ftpClient.listFiles(serverPath);
//		if (files.length == 0) {
//			listener.onDeleteProgress(MainActivity.FTP_FILE_NOTEXISTS);
//			return;
//		}
//		
//		boolean flag = true;
//		flag = ftpClient.deleteFile(serverPath);
//		if (flag) {
//			listener.onDeleteProgress(MainActivity.FTP_DELETEFILE_SUCCESS);
//		} else {
//			listener.onDeleteProgress(MainActivity.FTP_DELETEFILE_FAIL);
//		}
//		
//		this.closeConnect();
//		listener.onDeleteProgress(MainActivity.FTP_DISCONNECT_SUCCESS);
//		
//		return;
//	}
//
//	/**
//	 * 打开FTP服务.
//	 * 
//	 * @throws IOException
//	 */
//	public void openConnect() throws IOException {
//		// 中文转码
//		ftpClient.setControlEncoding("UTF-8");
//		int reply; // 服务器响应值
//		// 连接至服务器
//		ftpClient.connect(hostName, serverPort);
//		// 获取响应值
//		reply = ftpClient.getReplyCode();
//		if (!FTPReply.isPositiveCompletion(reply)) {
//			// 断开连接
//			ftpClient.disconnect();
//			throw new IOException("connect fail: " + reply);
//		}
//		// 登录到服务器
//		ftpClient.login(userName, password);
//		// 获取响应值
//		reply = ftpClient.getReplyCode();
//		if (!FTPReply.isPositiveCompletion(reply)) {
//			// 断开连接
//			ftpClient.disconnect();
//			throw new IOException("connect fail: " + reply);
//		} else {
//			// 获取登录信息
//			FTPClientConfig config = new FTPClientConfig(ftpClient
//					.getSystemType().split(" ")[0]);
//			config.setServerLanguageCode("zh");
//			ftpClient.configure(config);
//			// 使用被动模式设为默认
//			ftpClient.enterLocalPassiveMode();
//			// 二进制文件支持
//			ftpClient
//					.setFileType(org.apache.commons.net.ftp.FTP.BINARY_FILE_TYPE);
//		}
//	}
//
//	/**
//	 * 关闭FTP服务.
//	 * 
//	 * @throws IOException
//	 */
//	public void closeConnect() throws IOException {
//		if (ftpClient != null) {
//			// 退出FTP
//			ftpClient.logout();
//			// 断开连接
//			ftpClient.disconnect();
//		}
//	}
//	
//	/*
//	 * 上传进度监听
//	 */
//	public interface UploadProgressListener {
//		public void onUploadProgress(String currentStep, long uploadSize, File file);
//	}
//
//	/*
//	 * 下载进度监听
//	 */
//	public interface DownLoadProgressListener {
//		public void onDownLoadProgress(String currentStep, long downProcess, File file);
//	}
//
//	/*
//	 * 文件删除监听
//	 */
//	public interface DeleteFileProgressListener {
//		public void onDeleteProgress(String currentStep);
//	}
//
//
//}
