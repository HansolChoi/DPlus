package Socket;

public class Socket {
    static{
        System.loadLibrary("nativeLib");
    }
    
    public native int CreateSocket(int port);
    public native int SocketSend(String data, int len, int port);
    public native String SocketReceive(int port);
    public native void SocketStop(int port);
	
    public int Create(int port){
    	CreateSocket(port);
    	return 1;
    }
    
    public int Send(String data, int len, int port){
    	return SocketSend(data, len, port);
    }
    
    public String Receive(int port){
    	return SocketReceive(port);
    }
    
    public void Stop(int port){
    	SocketStop(port);
    }
}
