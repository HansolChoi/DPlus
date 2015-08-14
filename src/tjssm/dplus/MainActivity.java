package tjssm.dplus;

import android.app.Activity;
import android.os.Build;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

import tjssm.NativeCaller.*;

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
    public NativeCaller native_caller;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) 
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Component Initialized
        Component_Init();
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
        ip_field            .setText("210.118.64.165");
        user_level			.setText("0%");
        system_level		.setText("0KB");
        amount_level        .setText("0%");
        
        device_name         .setText(Build.MODEL);
        sdk_level           .setText(Build.VERSION.RELEASE);

        // event transaction
        connect_button      .setOnClickListener(this);
        disconnect_button   .setOnClickListener(this);
    }

    @Override
    public void onClick(View v) 
    {
        switch(v.getId()) 
        {
            case R.id.connect_button:
            	native_caller.ServerCommandListen();
            	native_caller.LogcatExcute();
            	native_caller.CommandLineTool();
            	native_caller.ResourceExtract();
                break;
            case R.id.disconnect_button:
            	native_caller.NativeServiceStop();
                break;
        }
    }
}
