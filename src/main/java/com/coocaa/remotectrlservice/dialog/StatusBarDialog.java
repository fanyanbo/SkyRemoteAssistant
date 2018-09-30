package com.coocaa.remotectrlservice.dialog;

import com.coocaa.remotectrlservice.R;
import com.skyworth.theme.SkyThemeEngine;
import com.skyworth.ui.blurbg.BlurBgLayout;
import com.skyworth.ui.define.SkyTextSize;
import com.skyworth.util.SkyScreenParams;

import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.view.Gravity;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout.LayoutParams;
import android.widget.TextView;

public class StatusBarDialog extends Dialog {

	private static StatusBarDialog instance = null;  
	private Context mContext = null;
	private FrameLayout shellLayout = null;
	private FrameLayout mainLayout = null;
	private BlurBgLayout mBgLayout = null;
	private ImageView imgView = null;
	private TextView textView1 = null;
	private TextView textView2 = null;
	private TextView textView3 = null;
	private TextView textView4 = null;
	private TextView textView5 = null;
	
	public static  StatusBarDialog getInstance(Context context)
	{
		if (instance == null) {     
			instance = new StatusBarDialog(context);        
		}    
		return instance;    
	}

	private StatusBarDialog(Context context) {
		super(context,R.style.dialog);
		// TODO Auto-generated constructor stub		
		Window dialogWindow = getWindow();
        dialogWindow.setGravity(Gravity.LEFT | Gravity.TOP);
        dialogWindow.setType(WindowManager.LayoutParams.TYPE_SYSTEM_OVERLAY);
	
        SkyThemeEngine.getInstance().registerDialog(this);
        
        mContext = context;
        SkyScreenParams.getInstence(mContext);
        
        shellLayout = new FrameLayout(context);
               
        mainLayout = new FrameLayout(context);
//        mainLayout.setBackgroundResource(R.drawable.ui_sdk_other_page_bg_blur);
        mBgLayout = new BlurBgLayout(context);
        mBgLayout.setPageType(BlurBgLayout.PAGETYPE.OTHER_PAGE);
        FrameLayout.LayoutParams bgParams = new FrameLayout.LayoutParams(
        		LayoutParams.MATCH_PARENT,
        		LayoutParams.MATCH_PARENT);
        mBgLayout.setLayoutParams(bgParams);
        mBgLayout.setBgAlpha(1.0f);
        mainLayout.addView(mBgLayout);
           
        imgView = new ImageView(context);
        imgView.setBackgroundResource(R.drawable.icon);
      
        FrameLayout.LayoutParams lp1 = new FrameLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        lp1.leftMargin = SkyScreenParams.getInstence(mContext).getResolutionValue(39);
        lp1.gravity = Gravity.CENTER_VERTICAL;
        mainLayout.addView(imgView, lp1);
        
        
        textView1 = new TextView(context);
        textView1.setText("已被远程连接，快速按");
        textView1.setFocusable(false);
        textView1.setTextSize(SkyScreenParams.getInstence(mContext).getTextDpiValue(SkyTextSize.t_4));
        textView1.setTextColor(mContext.getResources().getColor(R.color.c_3));
        textView1.setGravity(Gravity.CENTER_VERTICAL);
        
        FrameLayout.LayoutParams lp2 = new FrameLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        lp2.leftMargin = SkyScreenParams.getInstence(mContext).getResolutionValue(120);
        lp2.gravity = Gravity.CENTER_VERTICAL;
        mainLayout.addView(textView1, lp2);

        
        textView2 = new TextView(context);
        textView2.setText("3");
        textView2.setFocusable(false);
        textView2.setTextSize(SkyScreenParams.getInstence(mContext).getTextDpiValue(SkyTextSize.t_4));
        textView2.setTextColor(mContext.getResources().getColor(R.color.a_7));
        textView2.setGravity(Gravity.CENTER);
        
        FrameLayout.LayoutParams lp4 = new FrameLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        lp4.leftMargin = SkyScreenParams.getInstence(mContext).getResolutionValue(490);
        lp4.gravity = Gravity.CENTER_VERTICAL;
        mainLayout.addView(textView2, lp4);
        
        textView3 = new TextView(context);
        textView3.setText("次");
        textView3.setFocusable(false);
        textView3.setTextSize(SkyScreenParams.getInstence(mContext).getTextDpiValue(SkyTextSize.t_4));
        textView3.setTextColor(mContext.getResources().getColor(R.color.c_3));
        textView3.setGravity(Gravity.CENTER);
        
        FrameLayout.LayoutParams lp5 = new FrameLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        lp5.leftMargin = SkyScreenParams.getInstence(mContext).getResolutionValue(520);
        lp5.gravity = Gravity.CENTER_VERTICAL;
        mainLayout.addView(textView3, lp5);
        
        textView4 = new TextView(context);
        textView4.setText("【返回键】");
        textView4.setFocusable(false);
        textView4.setTextSize(SkyScreenParams.getInstence(mContext).getTextDpiValue(SkyTextSize.t_4));
        textView4.setTextColor(mContext.getResources().getColor(R.color.a_7));
        textView4.setGravity(Gravity.CENTER);
        
        FrameLayout.LayoutParams lp6 = new FrameLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        lp6.gravity = Gravity.CENTER_VERTICAL;
        lp6.leftMargin = SkyScreenParams.getInstence(mContext).getResolutionValue(550);
        mainLayout.addView(textView4, lp6);
        
        textView5 = new TextView(context);
        textView5.setText("可断开连接");
        textView5.setFocusable(false);
        textView5.setTextSize(SkyScreenParams.getInstence(mContext).getTextDpiValue(SkyTextSize.t_4));
        textView5.setTextColor(mContext.getResources().getColor(R.color.c_3));
        textView5.setGravity(Gravity.CENTER);
        
        FrameLayout.LayoutParams lp7 = new FrameLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        lp7.gravity = Gravity.CENTER_VERTICAL;
        lp7.leftMargin = SkyScreenParams.getInstence(mContext).getResolutionValue(720);
        mainLayout.addView(textView5, lp7);
        
				
		this.setOnDismissListener(new OnDismissListener() {
			
			@Override
			public void onDismiss(DialogInterface arg0) {
				// TODO Auto-generated method stub

			}
		});
		
		this.setOnShowListener(new OnShowListener() {
			
			@Override
			public void onShow(DialogInterface arg0) {
				// TODO Auto-generated method stub
				refreshOnThemeChanged();
			}
		});
		
		this.setOnCancelListener(new OnCancelListener() {
			
			@Override
			public void onCancel(DialogInterface arg0) {
				// TODO Auto-generated method stub

			}
		});
		
        FrameLayout.LayoutParams lp8 = new FrameLayout.LayoutParams(
        		SkyScreenParams.getInstence(context).getResolutionValue(940), 
        		SkyScreenParams.getInstence(context).getResolutionValue(100));
        lp8.topMargin = SkyScreenParams.getInstence(mContext).getResolutionValue(904);
        lp8.leftMargin = SkyScreenParams.getInstence(mContext).getResolutionValue(52);
        
        shellLayout.addView(mainLayout,lp8);
        
		setContentView(shellLayout,new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));
	}
	
	public boolean isShown(){
		return this.isShowing();
	}
	  
	public void showDialog(){
		this.show();
	}
	
	public void hideDialog(){
		this.hide();
	}
	
	public void cancelDialog(){
		this.cancel();
	}
	
	private void refreshOnThemeChanged() {
		// TODO Auto-generated method stub
		textView1.setTextColor(mContext.getResources().getColor(R.color.c_3));
		textView3.setTextColor(mContext.getResources().getColor(R.color.c_3));
		textView5.setTextColor(mContext.getResources().getColor(R.color.c_3));
	}
}
