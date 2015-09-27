package dplus;

import android.app.Activity;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.StrictMode;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;
import tjssm.dplus.R;
import Socket.*;
import NativeService.*;

public class MainActivity extends Activity implements OnClickListener 
{
    /* Component */
    public static  EditText        ip_field;
    public static   TextView        server_status;
    private         LinearLayout    connect_button;
    private         LinearLayout    disconnect_button;
    public static   TextView        device_name;
    public static   TextView        sdk_level;
    public static   TextView        user_level;
    public static   TextView        system_level;
    public static   TextView        amount_level;
    
    public static ClientSocket singleton_socket = ClientSocket.getInstance();
    public static int ScreenWidth, ScreenHeight;
    public static Context m_context;
    
	public static String ToastMessage;
	public static Handler handler;
	
	public NativeService ns;
    public int test;
    @Override
    protected void onCreate(Bundle savedInstanceState) 
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        m_context = this;
        Thread_Handling();

        // Component Initialized
        Component_Init();
    }

    @Override
    protected void onDestroy() {
    	singleton_socket.socket_closed();
        super.onDestroy();
    }
    
    public void Thread_Handling() {
        if(Build.VERSION.SDK_INT > 9) {
            StrictMode.ThreadPolicy  policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
            StrictMode.setThreadPolicy(policy);
        }
    }

    public void Component_Init() 
    {
        // init
        ip_field 			= (EditText) 		findViewById(R.id.ip_field);
        server_status 		= (TextView)		findViewById(R.id.server_status);
        connect_button 		= (LinearLayout) 	findViewById(R.id.connect_button);
        disconnect_button 	= (LinearLayout) 	findViewById(R.id.disconnect_button);
        device_name 		= (TextView) 		findViewById(R.id.device_name);
        sdk_level 			= (TextView) 		findViewById(R.id.sdk_level);
        user_level 			= (TextView) 		findViewById(R.id.user_level);
        system_level 		= (TextView) 		findViewById(R.id.system_level);
        amount_level 		= (TextView) 		findViewById(R.id.amount_level);

        // layout arranged
        ip_field        .setGravity(Gravity.CENTER);
        server_status   .setGravity(Gravity.CENTER);
        device_name     .setGravity(Gravity.CENTER);
        sdk_level       .setGravity(Gravity.CENTER);
        user_level      .setGravity(Gravity.CENTER);
        system_level    .setGravity(Gravity.CENTER);
        amount_level    .setGravity(Gravity.CENTER);

        // data setting
        ip_field            .setText("192.168.137.16"/*"1.214.224.77"*/);
        user_level			.setText("0%");
        system_level		.setText("0KB");
        amount_level        .setText("0%");
        
        device_name         .setText(Build.MODEL);
        sdk_level           .setText(Build.VERSION.RELEASE);

        // event transaction
        connect_button      .setOnClickListener(this);
        disconnect_button   .setOnClickListener(this);
        
        ns = new NativeService();
        test = 0;
        GetWindow(); // 스크린 사이즈 구하기
    }

    public void GetWindow()
    {
		DisplayMetrics displayMetrics = new DisplayMetrics();
		getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
		ScreenWidth = displayMetrics.widthPixels;
		ScreenHeight = displayMetrics.heightPixels;
    }
    
 
    @Override
    public void onClick(View v) 
    {
        switch(v.getId()) 
        {
            case R.id.connect_button:
            	ns.RecordStart(ScreenWidth, ScreenHeight);
            	//requestConnect();
            	/*if(test == 0){
            		test = 1;
            		Log.i("onClick", "testing");
            		ns.PlayInputTest(ScreenWidth, ScreenHeight, ScreenWidth, ScreenHeight);
            		Log.i("onClick", "test end");
            		test = 0;
            	}*/
                break;
            case R.id.disconnect_button:
            	ns.RecordStop();
            	//requestDisconnect();
                break;
        }
    }

    public static void requestConnect(){
        try {                	
            if(ClientSocket.isConnected == false) {                    
            	singleton_socket.socket_init(Build.MODEL, ip_field.getText().toString());
                singleton_socket.socket_opened();
                if(ClientSocket.isConnected == true) server_status.setText("연결됨");
                else server_status.setText("열결실패");
            }                    
        }
        catch(Exception e) { e.printStackTrace(); }
    }

    public static void requestDisconnect(){
        try {                	
            if(ClientSocket.isConnected == true ) {                    	
                singleton_socket.socket_closed();
                if(ClientSocket.isConnected == false) server_status.setText("연결끊어짐");                        
            }                   
        }
        catch(Exception e) { e.printStackTrace(); }
    }
    
    
    public static void toast(String message) {
        ToastMessage = message;
  
        try {
            handler = new Handler(Looper.getMainLooper());
            handler.post(new Runnable() {
                public void run() {
                    Toast.makeText(MainActivity.m_context, ToastMessage, Toast.LENGTH_SHORT)
                            .show();
                }
            });
            return;
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}