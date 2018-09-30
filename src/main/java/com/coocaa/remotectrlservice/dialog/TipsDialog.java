package com.coocaa.remotectrlservice.dialog;

import java.lang.ref.WeakReference;

import com.coocaa.remotectrlservice.R;
import com.skyworth.theme.SkyThemeEngine;
import com.skyworth.ui.blurbg.BlurBgLayout;
import com.skyworth.util.SkyScreenParams;

import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Handler;
import android.os.Message;
import android.view.Gravity;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.TextView;
import android.widget.FrameLayout.LayoutParams;

public class TipsDialog extends Dialog {

	private static TipsDialog instance = null;  
	private MyHandler mHandler;

    private FrameLayout contentLayout;
    private Context mContext;
    private TextView toastTextView;
    private FrameLayout shadowLayout;
    private FrameLayout blurBgLayout;
    private FrameLayout.LayoutParams shadowParams;
    private BlurBgLayout mBgLayout = null;
	
	public static  TipsDialog getInstance(Context context)
	{
		if (instance == null) {     
			instance = new TipsDialog(context);        
		}    
		return instance;    
	}

	private TipsDialog(Context context) {
		super(context,R.style.dialog);
		// TODO Auto-generated constructor stub		
		Window dialogWindow = getWindow();
        dialogWindow.setGravity(Gravity.CENTER);
        dialogWindow.setType(WindowManager.LayoutParams.TYPE_SYSTEM_OVERLAY);
        
        SkyThemeEngine.getInstance().registerDialog(this);
        
        if (mHandler == null)
            mHandler = new MyHandler(this);
        
        mContext = context;
         
        // contentLayout 全屏透明使用
        contentLayout = new FrameLayout(mContext);
        contentLayout.setFocusable(false);

        // shadowLayout 阴影布局使用
        shadowLayout = new FrameLayout(mContext);
        shadowParams = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.WRAP_CONTENT,
                FrameLayout.LayoutParams.WRAP_CONTENT);
        shadowParams.gravity = Gravity.CENTER_HORIZONTAL;
        shadowParams.topMargin = SkyScreenParams.getInstence(mContext).getResolutionValue(780);
        shadowLayout.setBackgroundResource(R.drawable.ui_sdk_toast_shadow_no_bg);

     //   blurBgLayout = new FrameLayout(mContext);
     //   blurBgLayout.setBackgroundResource(R.drawable.ui_sdk_other_page_bg_blur);
     //   blurBgLayout.setVisibility(View.GONE);
        mBgLayout = new BlurBgLayout(context);
        mBgLayout.setPageType(BlurBgLayout.PAGETYPE.OTHER_PAGE);
        FrameLayout.LayoutParams bgParams = new FrameLayout.LayoutParams(
        		SkyScreenParams.getInstence(mContext).getResolutionValue(540),
        		SkyScreenParams.getInstence(mContext).getResolutionValue(70));
        mBgLayout.setBgAlpha(1.0f);
        
        shadowLayout.addView(mBgLayout, bgParams);
        
        toastTextView = new TextView(mContext);
        FrameLayout.LayoutParams toastTextViewParams = new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.WRAP_CONTENT, FrameLayout.LayoutParams.WRAP_CONTENT);
        toastTextView.setText(R.string.exit_control_tip);
        toastTextView.setSingleLine(true);
        toastTextView.setFocusable(false);
        toastTextView.setGravity(Gravity.CENTER);
        toastTextView.setPadding(SkyScreenParams.getInstence(mContext).getResolutionValue(70),
                SkyScreenParams.getInstence(mContext).getResolutionValue(12), SkyScreenParams
                        .getInstence(mContext).getResolutionValue(70),
                SkyScreenParams.getInstence(mContext).getResolutionValue(12));
        toastTextView.setTextSize(SkyScreenParams.getInstence(mContext).getTextDpiValue(32));
        toastTextView.setTextColor(mContext.getResources().getColor(R.color.c_3));
   //     toastTextView.setVisibility(View.INVISIBLE);

        shadowLayout.addView(toastTextView, toastTextViewParams);
      
        contentLayout.addView(shadowLayout, shadowParams);
        
        this.setContentView(contentLayout, new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.WRAP_CONTENT, FrameLayout.LayoutParams.WRAP_CONTENT));

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
	}
	
	public boolean isShown(){
		return this.isShowing();
	}
	
	public void showDialog(){
//        toastTextView.setText(R.string.exit_control_tip);
//        toastTextView.post(new Runnable()
//        {
//            @Override
//            public void run()
//            {
//                FrameLayout.LayoutParams blue_p = (android.widget.FrameLayout.LayoutParams) blurBgLayout
//                        .getLayoutParams();
//                blue_p.width = toastTextView.getWidth();
//                blue_p.height = toastTextView.getHeight();
//                blurBgLayout.setLayoutParams(blue_p);
//                blurBgLayout.setVisibility(View.VISIBLE);
//            }
//        });
//        
//		shadowParams = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.WRAP_CONTENT,
//                FrameLayout.LayoutParams.WRAP_CONTENT);
//		shadowParams.gravity = Gravity.CENTER_HORIZONTAL;
//		shadowParams.topMargin = SkyScreenParams.getInstence(mContext).getResolutionValue(780);
//        shadowLayout.setLayoutParams(shadowParams);
//        toastTextView.setVisibility(View.VISIBLE);
		this.show();	
		if (mHandler.hasMessages(0))
            mHandler.removeMessages(0);
        mHandler.sendEmptyMessageDelayed(0, 3000);
	}
	
	public void hideDialog(){
		this.hide();
	}
	
	public void cancelDialog(){
		this.cancel();
	}
	
    @Override
    public void dismiss()
    {
        if (mHandler.hasMessages(0))
            mHandler.removeMessages(0);
        super.dismiss();
    }
	
    private static class MyHandler extends Handler
    {
        private final WeakReference<TipsDialog> mView;

        public MyHandler(TipsDialog view)
        {
            mView = new WeakReference<TipsDialog>(view);
        }

        @Override
        public void handleMessage(Message msg)
        {
        	TipsDialog view = mView.get();
            if (view != null)
            {
                switch (msg.what)
                {
                    case 0:
                        if (view.isShowing())
                            view.dismiss();
                        break;

                    default:
                        break;
                }
            }
        }
    }
    
	private void refreshOnThemeChanged() {
		// TODO Auto-generated method stub
		toastTextView.setTextColor(mContext.getResources().getColor(R.color.c_3));
	}
}
