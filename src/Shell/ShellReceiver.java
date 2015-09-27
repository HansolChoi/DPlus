package Shell;

import Socket.ClientSocket;
import Shell.Shell;
import android.util.Log;

public class ShellReceiver extends Thread {
	public String receive;
	public Shell shell;
	
	public ShellReceiver(){
		shell = new Shell();
		shell.setStream();
	}
	
	public void ShellCmd(String cmd)
	{
		Log.d("Shell", "cmd : " + cmd);
		shell.stdin.println(cmd);
		shell.stdin.flush();
	}
	
	@Override
	public void run()
	{
		try{
			while(ClientSocket.isConnected == true)
			{ 
				Thread.sleep(200);

				if(ClientSocket.input_stream_shell.available() != 0)
				{
					receive = ClientSocket.input_stream_shell.readUTF();
					Log.d("REC", receive);
					ShellCmd(receive);
				}
			}
		}catch(Exception e){
			e.printStackTrace();
		}
	}	
}
