package Socket;

public class Socket {
    static{
        System.loadLibrary("nativeLib");
    }
    
    public native int CreateSocket(String ip, int port);
    public native int SocketSends(String data, int len, int port);
    public native String SocketReceives(int port);
    public native void SocketStop(int port);
}
