package Shell;

import Shell.Shell;
import android.util.Log;

public class ShellReceiver {
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
}
