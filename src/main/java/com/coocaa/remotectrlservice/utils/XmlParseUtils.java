package com.coocaa.remotectrlservice.utils;
//package com.coocaa.tvagentservice.utils;
//
//import java.io.File;
//
//import javax.xml.parsers.DocumentBuilder;
//import javax.xml.parsers.DocumentBuilderFactory;
//
//import org.w3c.dom.Document;
//import org.w3c.dom.Element;
//import org.w3c.dom.Node;
//import org.w3c.dom.NodeList;
//
//import com.coocaa.tvagentservice.GlobalData;
//
//import android.util.Log;
//
//public class XmlParseUtils {
//
//	private static XmlParseUtils instance = null;
//	 public static final String TAG = "fyb";
//	
//	public static XmlParseUtils getInstance()
//	{
//		if (instance == null)
//			instance = new XmlParseUtils();
//		return instance;
//	}
//	
//	public boolean load(String xmlfileName)
//	{
//		boolean ret = loadXmlFile(xmlfileName);
//		return ret;
//	}
//	
//	private boolean loadXmlFile(String xmlfileName)
//	{
//		if(xmlfileName == null || xmlfileName == "")
//		{
//			Log.i("fyb", "load disk file param error");
//			return false;
//		}
//		boolean ret;
//		try 
//		{
//			Document doc = null;
//			DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();  
//	        DocumentBuilder db = dbf.newDocumentBuilder();  
//	        
//	        Log.i("fyb", "load disk file");
//	        doc = db.parse(new File(xmlfileName));
//	        
//	        if (doc == null)
//	        {
//	        	Log.e("fyb", "xml parse failed.");
//        		return false;
//	        }
//	        
//	        // 获得根元素结点
//	        Element root = doc.getDocumentElement();
//	          
//	        NodeList nodeList = root.getChildNodes();
//	        for(int i = 0; i < nodeList.getLength(); i++)  
//	        {  
//	            Node node = nodeList.item(i);  
//	            if(node.getNodeType() == Document.ELEMENT_NODE){
//	            	Element element = (Element)nodeList.item(i);
//	            	Log.i(TAG,"server ip = " + element.getAttribute("ip") + ", port = " + element.getAttribute("port"));
//	            	GlobalData.getInstance().setServerIp(element.getAttribute("ip"));
//	            }
//	        }
//	        
//	        ret = true;
//		}
//		catch (Exception e)
//		{
//			Log.e("fyb", "load xml fail : " + xmlfileName + ",error info : " + e.getMessage());
//			ret = false;
//		}
//		return ret;
//	}
//}
