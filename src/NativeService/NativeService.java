package NativeService;

import dplus.MainActivity;
import android.app.Activity;
import android.content.Context;
import android.graphics.Point;
import android.os.Handler;
import android.os.Looper;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import Shell.ShellReceiver;

public class NativeService{
    
    public static int SocketAvailable = 0;
    public static float cpu_msg, bat_msg;
    public static int mem_msg;
    public static int device_width, device_height;
    
	static{
		System.loadLibrary("NativeService");
	}

	public native void PlayInputTest(int Origin_Width, int Origin_Height, 
			int Device_Width, int Device_Height);
	public native void RecordStart(int Width, int Height);
	public native void RecordStop();
	
    public native void nativeSocketInit(String ip);
    public native void nativeSocketClose();
    public native int nativeDisplay(int index);
    
    public void Display(){
    	cpu_msg = nativeDisplay(1)/100;
    	
    	mem_msg = nativeDisplay(5);
    	
    	bat_msg = nativeDisplay(6);
    	
    	Handler handler = new Handler(Looper.getMainLooper());
    	handler.post(new Runnable() {
    		@Override
    		public void run() {
    			MainActivity.user_level.setText(cpu_msg + "%");
    			MainActivity.system_level.setText(mem_msg + "KB");
    			MainActivity.amount_level.setText(bat_msg + "%");
    		}
    	});
    }
}