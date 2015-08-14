package tjssm.ServerListen;

public class NativeCaller {
	
	static
	{
		System.loadLibrary("nativeLib");
	}
	
	public native void ServerCommandListen(String ip, int port);
	public native void LogcatExcute();
	public native void CommandLineTool();
	public native void ResourceExtract();
	public native void NativeServiceStop();
}
