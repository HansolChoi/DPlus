package Socket;

import MonkeyTest.MonkeyResult;
import android.content.Context;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.os.Vibrator;
import android.util.Log;
import android.widget.Toast;

import java.io.*;

import Socket.ClientSocket;
import dplus.MainActivity;

public class FileReceiver extends Thread {
	String data_connect;
	String data_apk_monkey;
	String package_name;

	public static final int FULL_LIVE_TIME = 100;
	public int liveTime = 100;
	public final String RecordFile = "sdcard/events.txt";
	private static Context mContext;

	
	public static ClientSocket singleton_socket = ClientSocket.getInstance();

	public void chargeLiveTime() {
		liveTime = FULL_LIVE_TIME;
	}

	public boolean consumeLiveTime() {
		liveTime--;
		try {
			Handler handler = new Handler(Looper.getMainLooper());
			handler.post(new Runnable() {
				public void run() {
					MainActivity.server_status.setText("연결됨(" + liveTime + ")");
				}
			});
		} catch (Exception e) {
			e.printStackTrace();
		}
		return (liveTime > 0);
	}

	@Override
	public void run() {
		while (ClientSocket.isConnected == true) {
			try {
				// socket input stream check
				if (!consumeLiveTime()) {
					try {
						Handler handler = new Handler(Looper.getMainLooper());
						handler.post(new Runnable() {
							public void run() {
								MainActivity.server_status.setText("서버 응답없음");
							}
						});
					} catch (Exception e) {
						e.printStackTrace();
					}
					singleton_socket.socket_closed();
				}
				if (ClientSocket.input_stream_connect.available() != 0) {
					data_connect = ClientSocket.input_stream_connect.readUTF();
					Log.d("bomb", "data_apk_monkey : " + data_connect);
					if (data_connect.equals("-ConnectionTest-")) {					
						chargeLiveTime();
						ClientSocket.output_stream_connect.writeUTF("alive");
						ClientSocket.output_stream_connect.flush();
					} else if (data_connect.equals("-ServerClosed-")) {
						try {
							Handler handler = new Handler(Looper.getMainLooper());
							handler.post(new Runnable() {
								public void run() {
									MainActivity.server_status.setText("서버 종료");
								}
							});
						} catch (Exception e) {
							e.printStackTrace();
						}
						singleton_socket.socket_closed();
						break;
					}
				}

				if(singleton_socket.native_obj.SocketAvailable != 0){
					singleton_socket.native_obj.Display();
				}
				// apk_monkey input stream check
				if (ClientSocket.input_stream_apk_monkey.available() != 0) {
					data_apk_monkey = ClientSocket.input_stream_apk_monkey
							.readUTF();

					// monkey test input
					Log.d("bomb", "data_apk_monkey : " + data_apk_monkey);
					if (data_apk_monkey.equals("-SendMonkeyTestCmd-")) {
						MainActivity.toast("Monkey-Test start!");
						Call_Monkey_Command();
					}
					// apk install input
					else if (data_apk_monkey.equals("-SendAPK-")) {
						MainActivity.toast("Install APK!");
						Apk_Receiver();
						// install
						Apk_Install();
						// execute
						Apk_Execute();
					} else if (data_apk_monkey.equals("-PlayAutoInputTest-")) {
						MainActivity.toast("PlayAutoInputTest!");
						
						int Origin_Width = ClientSocket.input_stream_apk_monkey
								.readInt();
						int Origin_Height = ClientSocket.input_stream_apk_monkey
								.readInt();	
						long full = ClientSocket.input_stream_apk_monkey
								.readLong();
						
						int Device_Width = MainActivity.ScreenWidth;
						int Device_Height = MainActivity.ScreenHeight;
						//81.7kb
						// 파일 저장.
						File f = new File(RecordFile);
						BufferedOutputStream bos = new BufferedOutputStream(
								new FileOutputStream(f));
						
						// 바이트 데이터를 전송받으면서 기록
						int len, size = 32768;
						byte[] data = new byte[size];
						while (full > 0) {
							len = ClientSocket.input_stream_apk_monkey
									.read(data);
							bos.write(data, 0, len);
							full -= len;
						}
						bos.flush();
						bos.close();
						
						singleton_socket.native_obj.PlayInputTest(Origin_Width, Origin_Height, Device_Width, Device_Height);

						MainActivity.toast("PlayAutoInputTest Stop!");
					}else if(data_apk_monkey.equals("-StartAutoInput-")){
						MainActivity.toast("Recodring start!");
						
						singleton_socket.native_obj.RecordStart(MainActivity.ScreenWidth, MainActivity.ScreenHeight);
					}else if(data_apk_monkey.equals("-StopAutoInput-")){
						// 중지
						singleton_socket.native_obj.RecordStop();
						
						SendRecordFile(MainActivity.ScreenWidth, MainActivity.ScreenHeight);
						MainActivity.toast("Recodring stop!");
					}
				}
				Thread.sleep(200);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

	public void Call_Monkey_Command() {
		try {
			String cmd = ClientSocket.input_stream_apk_monkey.readUTF();
			Log.d("bomb", "cmd : " + cmd);

			ProcessBuilder pb = new ProcessBuilder("su"); // rooting
			pb.redirectErrorStream(true);
			Process p = pb.start();
			OutputStreamWriter osw = new OutputStreamWriter(p.getOutputStream());
			osw.write(cmd);
			osw.flush();
			InputStreamReader isr = new InputStreamReader(p.getInputStream());
			Thread t1 = new Thread(new MonkeyResult(isr));
			t1.start();
			osw.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void Apk_Receiver() {

		try {
			package_name = ClientSocket.input_stream_apk_monkey.readUTF();

			long full = ClientSocket.input_stream_apk_monkey.readLong();
			
			String dirPath = Environment.getExternalStorageDirectory() + "/DP";
			File targetDir = new File(dirPath);
			if(!targetDir.exists()) {
				targetDir.mkdirs();
			}

			File f = new File(Environment.getExternalStorageDirectory(),
					"DP/test.apk");
			if(!f.exists()) {
				f.createNewFile();
			}
			
			BufferedOutputStream bos = new BufferedOutputStream(
					new FileOutputStream(f));

			int len;
			byte[] data = new byte[32768];
			while (full > 0) {
				len = ClientSocket.input_stream_apk_monkey.read(data);
				bos.write(data, 0, len);
				full -= len;
			}
			bos.flush();
			bos.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void Apk_Install() {
		try {
			String cmd = "pm install -r "
					+ Environment.getExternalStorageDirectory().toString()
					+ "/DP/test.apk";
			Log.d("bomb", "command : " + cmd);
			Runtime runtime = Runtime.getRuntime();
			Process p = runtime.exec("su");
			OutputStreamWriter osw = new OutputStreamWriter(
					(p.getOutputStream()));
			osw.write(cmd);
			osw.flush();
			osw.close();
			p.waitFor();
			if (p.exitValue() == 0) {
				Log.d("bomb", "install success");
			} // success
			else {
				Log.d("bomb", "install fail");
			} // fail
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void Apk_Execute() {
		try {
			String cmd = "monkey -p "
					+ package_name
					+ " -s 69233 --pct-touch 0 --pct-motion 0 --pct-trackball 100 --pct-nav 0 --pct-majornav 0 --pct-syskeys 0 --pct-appswitch 0 --pct-anyevent 0 --throttle 10 -v 1";
			Log.d("bomb", "command " + cmd);
			Runtime runtime = Runtime.getRuntime();
			Process p = runtime.exec("su");
			OutputStreamWriter osw = new OutputStreamWriter(p.getOutputStream());
			osw.write(cmd);
			osw.flush();
			osw.close();
			p.waitFor();
			if (p.exitValue() == 0) {
				Log.d("bomb", "execute success");
			} // success
			else {
				Log.d("bomb", "execute fail");
			} // fail
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

    public void SendRecordFile(int Width, int Height) {
        new Thread(new SendRecordFileThread(Width, Height)).start();
    }

	public class SendRecordFileThread implements Runnable
	{
		 String path;
	     BufferedInputStream bis;
	     int ScreenWidth, ScreenHeight;
	     SendRecordFileThread(int width, int height){
	    	 path = RecordFile;
	         ScreenWidth = width;
	         ScreenHeight = height;
	     }
	     @Override
	     public void run() {
	    	 try {
	    		 File f = new File(path);
	             bis=new BufferedInputStream(new FileInputStream(f));
	             singleton_socket.output_stream_apk_monkey.writeUTF("-SendRecordFile-");
	             singleton_socket.output_stream_apk_monkey.flush();
	             singleton_socket.output_stream_apk_monkey.writeInt(ScreenWidth);
	             singleton_socket.output_stream_apk_monkey.flush();
	             singleton_socket.output_stream_apk_monkey.writeInt(ScreenHeight);
	             singleton_socket.output_stream_apk_monkey.flush();
	             singleton_socket.output_stream_apk_monkey.writeLong(f.length());
	             singleton_socket.output_stream_apk_monkey.flush();
	             int len, size = 32768, sum = 0;
	             byte[] data = new byte[size];
	             while ((len = bis.read(data)) != -1) {
	            	 sum += len;
	                 singleton_socket.output_stream_apk_monkey.write(data, 0, len);
	             }
	             singleton_socket.output_stream_apk_monkey.flush();
	             bis.close();
	    	 } catch (Exception e) {
	                e.printStackTrace();
	         }
	    }
	 } 
}
