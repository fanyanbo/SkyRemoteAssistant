package com.coocaa.remotectrlservice.dialog;

import com.coocaa.remotectrlservice.GlobalData;
import com.coocaa.remotectrlservice.JniUtils;
import com.coocaa.remotectrlservice.MainService;
import com.coocaa.remotectrlservice.R;
import com.skyworth.theme.SkyThemeEngine;
import com.skyworth.ui.blurbg.BlurBgLayout;
import com.skyworth.ui.define.SkyTextSize;
import com.skyworth.util.MyFocusFrame;
import com.skyworth.util.SkyScreenParams;

import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.CountDownTimer;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.FrameLayout.LayoutParams;

public class AuthorizedDialog extends Dialog {

	private static final String TAG = "RemoteCtrl";
	private static final int btnOk = 111;
	private static final int btnCancel = 222;

	private static AuthorizedDialog instance = null;  
	private Button mBtnFirst = null;
	private Button mBtnSecond = null;
	private TextView mTvTip1 = null;
	private TextView mTvTip2 = null;
	private ImageView mLine = null;
	private final int COUNT_DOWN_TIME = 61;
	private MyCountDownTimer mTimer = null; 
	
	private MyFocusFrame focusLayout = null;
	private int unfocusShaderSize = 0;
	private Context mContext;
	
	private FrameLayout mainLayout = null;
	private BlurBgLayout mBgLayout = null;
	private FrameLayout mContentLayout = null;
	private FrameLayout mShadowLayout = null;

		
	public static  AuthorizedDialog getInstance(Context context)
	{
		if (instance == null) {     
			instance = new AuthorizedDialog(context);        
		}    
		return instance;    
	}

	public AuthorizedDialog(Context context) {
		super(context,R.style.dialog);
		// TODO Auto-generated constructor stub		
		Window dialogWindow = getWindow();
        dialogWindow.setGravity(Gravity.CENTER_HORIZONTAL | Gravity.CENTER_VERTICAL);
        dialogWindow.setType(WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);
      
        SkyThemeEngine.getInstance().registerDialog(this);
        
        mContext = context;
        
        unfocusShaderSize = SkyScreenParams.getInstence(context).getResolutionValue(12);
        
        mainLayout = new FrameLayout(context);
        
        mShadowLayout = new FrameLayout(context);
        mShadowLayout.setBackgroundResource(R.drawable.ui_sdk_dialog_shadow);
        FrameLayout.LayoutParams shadowParams = new FrameLayout.LayoutParams(
        		SkyScreenParams.getInstence(mContext).getResolutionValue(1380),
        		SkyScreenParams.getInstence(mContext).getResolutionValue(835));
        shadowParams.gravity = Gravity.CENTER;
        mShadowLayout.setLayoutParams(shadowParams);
        
        mainLayout.addView(mShadowLayout);
        
        mBgLayout = new BlurBgLayout(context);
        mBgLayout.setPageType(BlurBgLayout.PAGETYPE.OTHER_PAGE);
        FrameLayout.LayoutParams bgParams = new FrameLayout.LayoutParams(
        		SkyScreenParams.getInstence(mContext).getResolutionValue(1200),
        		SkyScreenParams.getInstence(mContext).getResolutionValue(650));
        bgParams.gravity = Gravity.CENTER;
        mBgLayout.setLayoutParams(bgParams);
        mBgLayout.setBgAlpha(1.0f);
        
        mainLayout.addView(mBgLayout);

        focusLayout = new MyFocusFrame(mContext, SkyScreenParams.getInstence(mContext)
                .getResolutionValue(83 - unfocusShaderSize));
        focusLayout.needAnimtion(true);
        focusLayout.setImgResourse(R.drawable.ui_sdk_btn_focus_shadow_no_bg);
        
        FrameLayout.LayoutParams focus_p = new FrameLayout.LayoutParams(LayoutParams.WRAP_CONTENT,LayoutParams.WRAP_CONTENT);
        mainLayout.addView(focusLayout, focus_p);
        
        
        mContentLayout = new FrameLayout(context);
        FrameLayout.LayoutParams contentParams = new FrameLayout.LayoutParams(
        		SkyScreenParams.getInstence(mContext).getResolutionValue(1200),
        		SkyScreenParams.getInstence(mContext).getResolutionValue(650));
        contentParams.gravity = Gravity.CENTER;
        mContentLayout.setLayoutParams(contentParams);
        mainLayout.addView(mContentLayout);
        
        mTvTip1 = new TextView(context);
        mTvTip1.setText(R.string.tvdoctor_service);
        mTvTip1.setFocusable(false);
        mTvTip1.setTextSize(SkyScreenParams.getInstence(mContext).getTextDpiValue(SkyTextSize.t_5));
        mTvTip1.setTextColor(context.getResources().getColor(R.color.c_2));
        mTvTip1.setGravity(Gravity.CENTER);
        
        mTvTip2 = new TextView(context);
        mTvTip2.setText(R.string.control_request);
        mTvTip2.setFocusable(false);
        mTvTip2.setTextSize(SkyScreenParams.getInstence(mContext).getTextDpiValue(SkyTextSize.t_4));
        mTvTip2.setTextColor(context.getResources().getColor(R.color.c_4));
        mTvTip2.setGravity(Gravity.CENTER);
        
        
        mLine = new ImageView(context);
        mLine.setBackground(SkyThemeEngine.getInstance().getDrawable(R.drawable.ui_sdk_menu_title_line,true));
        mLine.setFocusable(false);

        mBtnFirst = new Button(context);
        mBtnFirst.setId(btnOk);
        mBtnFirst.setGravity(Gravity.CENTER);
        mBtnFirst.setText(R.string.auth);
        mBtnFirst.setTextSize(SkyScreenParams.getInstence(mContext).getTextDpiValue(SkyTextSize.t_4));
        mBtnFirst.setTextColor(context.getResources().getColor(R.color.c_3));
    //    mBtnFirst.setBackgroundResource(R.drawable.ui_sdk_btn_unfocus_big_shadow);
        mBtnFirst.setBackground(SkyThemeEngine.getInstance().getDrawable(R.drawable.ui_sdk_btn_unfocus_big_shadow, false));
        mBtnFirst.setFocusable(true);
        mBtnFirst.setOnClickListener(new clickListener());
        mBtnFirst.setOnFocusChangeListener(new focusListener());
        
        mBtnSecond = new Button(context);
        mBtnSecond.setId(btnCancel);
        mBtnSecond.setGravity(Gravity.CENTER);
        mBtnSecond.setText(R.string.refuse);
        mBtnSecond.setTextSize(SkyScreenParams.getInstence(mContext).getTextDpiValue(SkyTextSize.t_4));
        mBtnSecond.setTextColor(context.getResources().getColor(R.color.c_3));
        mBtnSecond.setBackgroundResource(R.drawable.ui_sdk_btn_unfocus_big_shadow);
		mBtnSecond.setOnClickListener(new clickListener());
		mBtnSecond.setOnFocusChangeListener(new focusListener());
                
        LayoutParams lp1 = new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        lp1.leftMargin = SkyScreenParams.getInstence(mContext).getResolutionValue(163);
        lp1.topMargin = SkyScreenParams.getInstence(mContext).getResolutionValue(106);
        mContentLayout.addView(mTvTip1, lp1);
        
        LayoutParams line = new LayoutParams(SkyScreenParams.getInstence(mContext).getResolutionValue(400), LayoutParams.WRAP_CONTENT);
        line.leftMargin = SkyScreenParams.getInstence(mContext).getResolutionValue(74);
        line.topMargin = SkyScreenParams.getInstence(mContext).getResolutionValue(171);
        mContentLayout.addView(mLine, line);
        
        LayoutParams lp2 = new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        lp2.leftMargin = SkyScreenParams.getInstence(mContext).getResolutionValue(163);
        lp2.topMargin = SkyScreenParams.getInstence(mContext).getResolutionValue(250);
        mContentLayout.addView(mTvTip2, lp2);
        
        LayoutParams lp3 = new LayoutParams(SkyScreenParams.getInstence(mContext).getResolutionValue(260), SkyScreenParams.getInstence(mContext).getResolutionValue(100));
        lp3.leftMargin = SkyScreenParams.getInstence(mContext).getResolutionValue(272);
        lp3.topMargin = SkyScreenParams.getInstence(mContext).getResolutionValue(405);
        mContentLayout.addView(mBtnFirst, lp3);
        
        LayoutParams lp4 = new LayoutParams(SkyScreenParams.getInstence(mContext).getResolutionValue(260),SkyScreenParams.getInstence(mContext).getResolutionValue(100));
        lp4.leftMargin = SkyScreenParams.getInstence(mContext).getResolutionValue(630);
        lp4.topMargin = SkyScreenParams.getInstence(mContext).getResolutionValue(405);
        mContentLayout.addView(mBtnSecond, lp4);

		setContentView(mainLayout,new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));
		
		mTimer = new MyCountDownTimer(COUNT_DOWN_TIME*1000, 1000); 
		
		this.setOnDismissListener(new OnDismissListener() {
			
			@Override
			public void onDismiss(DialogInterface arg0) {
				// TODO Auto-generated method stub
				mTimer.cancel();
			}
		});
		
		this.setOnShowListener(new OnShowListener() {
			
			@Override
			public void onShow(DialogInterface arg0) {
				// TODO Auto-generated method stub
				Log.i(TAG, "AuthorizedDialog......OnShowListener");
				refreshOnThemeChanged();
				mBtnFirst.requestFocus();
			}
		});
		
		this.setOnCancelListener(new OnCancelListener() {
			
			@Override
			public void onCancel(DialogInterface arg0) {
				// TODO Auto-generated method stub
				mTimer.cancel();
			}
		});
	}
	
	private class clickListener implements View.OnClickListener {
		@Override
		public void onClick(View v) {
			// TODO Auto-generated method stub
			int id = v.getId();
			Log.i(TAG, "clickListener id = " + v.getId());
			switch (id) 
			{
				case btnOk:
				{
					String ip = GlobalData.getInstance().getServerIp();
					String tvid = GlobalData.getInstance().getTvId();
					Log.i(TAG, "TV accept control request ip:" + ip + ",tvid:" + tvid);
					if(tvid == null)
					{
						if(MainService.instance != null)
							MainService.instance.setTvId();
						tvid = GlobalData.getInstance().getTvId();
						Log.i(TAG, "TV accept control request ip:" + ip + ",tvid:" + tvid);
					}
					if(ip == null)
						ip = GlobalData.getInstance().getDefaultServerIp();
					if (tvid != null && ip != null) 
					{
						JniUtils.startVnc(tvid, ip, true);
					}
					break;
				}
			case btnCancel:
				{
					String ip = GlobalData.getInstance().getServerIp();
					String tvid = GlobalData.getInstance().getTvId();
					if(tvid != null && ip != null){			
						JniUtils.startVnc(tvid,ip,false);
					}
					break;
				}
			}
			mTimer.cancel();
		//	hide();
			dismiss();
		}
	};
	
	private class focusListener implements View.OnFocusChangeListener {

		@Override
		public void onFocusChange(View v, boolean hasFocus) {
			// TODO Auto-generated method stub
			if(!hasFocus)
				return;
			int id = v.getId();
			Log.i(TAG, "clickListener id = " + v.getId());
			switch (id) {
			case btnOk:
				mBtnSecond.setBackground(SkyThemeEngine.getInstance().getDrawable(R.drawable.ui_sdk_btn_unfocus_big_shadow, false));
				mBtnFirst.setBackgroundResource(R.drawable.ui_sdk_shape_focus_white);
				mBtnFirst.setTextColor(mContext.getResources().getColor(R.color.a_5));
				mBtnSecond.setTextColor(mContext.getResources().getColor(R.color.c_3));
				focusLayout.changeFocusPos(mBtnFirst);
				break;
			case btnCancel:
				mBtnFirst.setBackground(SkyThemeEngine.getInstance().getDrawable(R.drawable.ui_sdk_btn_unfocus_big_shadow, false));
				mBtnSecond.setBackgroundResource(R.drawable.ui_sdk_shape_focus_white);
				mBtnSecond.setTextColor(mContext.getResources().getColor(R.color.a_5));
				mBtnFirst.setTextColor(mContext.getResources().getColor(R.color.c_3));
				focusLayout.changeFocusPos(mBtnSecond);
				break;
			}
		}
	};
	
	public void setTipContent(int rid){
		mTvTip2.setText(rid);
	}
	
	public boolean isShown(){
		return mTvTip2.isShown();
	}
	
	public void showDialog(){
		mTimer.start();
		this.show();
	}
	
	public void hideDialog(){
		this.hide();
	}
	
	public void cancelDialog(){
		this.cancel();
	}

	class MyCountDownTimer extends CountDownTimer{

		public MyCountDownTimer(long millisInFuture, long countDownInterval) {
			super(millisInFuture, countDownInterval);
			// TODO Auto-generated constructor stub
		}

		@Override
		public void onFinish() {
			// TODO Auto-generated method stub
			Log.i(TAG, "MyCountDownTimer onFinish"); 
			mBtnSecond.performClick();
		}

		@Override
		public void onTick(long millisUntilFinished) {
			// TODO Auto-generated method stub
			Log.i(TAG, millisUntilFinished + ""); 
			mBtnSecond.setText("拒绝(" + millisUntilFinished / 1000 + "s)");
		}
		
	}

	private void refreshOnThemeChanged() {
		// TODO Auto-generated method stub
		mTvTip1.setTextColor(mContext.getResources().getColor(R.color.c_2));
		mTvTip2.setTextColor(mContext.getResources().getColor(R.color.c_4));
		mLine.setBackground(SkyThemeEngine.getInstance().getDrawable(R.drawable.ui_sdk_menu_title_line,true));
		mBtnSecond.setBackground(SkyThemeEngine.getInstance().getDrawable(R.drawable.ui_sdk_btn_unfocus_big_shadow, false));
		mBtnSecond.setTextColor(mContext.getResources().getColor(R.color.c_3));
	}
	
}
