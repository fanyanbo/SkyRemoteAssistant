package com.coocaa.remotectrlservice;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

public class MainActivity extends Activity {

//    static {
//        System.loadLibrary("hello");
//    }
//
//    public native String sayHello();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
//        Log.i("RemoteCtrl", "MainActivity onCreate sayHello == " + sayHello());
        Log.i("RemoteCtrl", "MainActivity onCreate sayHello [test v1.1]");
    }
}
