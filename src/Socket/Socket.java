package Socket;

public class Socket {
    static{
        System.loadLibrary("nativeLib");
    }
    
    public native int CreateSocket();
    public native int SocketSend(String data, int len, int port);
    public native String SocketReceive(int port);
    public native void SocketStop();
	
    public int Create(){
    	CreateSocket();
    	return 1;
    }
    
    public int Send(String data, int len, int port){
    	return SocketSend(data, len, port);
    }
    
    public String Receive(int port){
    	return SocketReceive(port);
    }
    
    public void Stop(){
    	SocketStop();
    }
}
