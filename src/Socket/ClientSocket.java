package Socket;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;

import android.util.Log;
import NativeService.*;

/**
 * Created by namhoon on 2015-06- 19.
 */
public class ClientSocket {

    /* input stream */
    public static DataInputStream input_stream_connect;
    public static DataInputStream input_stream_apk_monkey;
    public static DataInputStream input_stream_shell;
    
    /* output stream */
    public static DataOutputStream output_stream_connect;
    public static DataOutputStream output_stream_apk_monkey;
    public static DataOutputStream output_stream_shell;
    
    /* Socket Spec */
    public String   m_id;
    public String   m_ip;
    public int      port;
    public Socket   socket_connect = null;
    public Socket   socket_apk_monkey;
    public Socket 	socket_shell;
    
    /* FLAG */
    public static boolean isConnected = false;

    /* Socket Class */
    private static volatile ClientSocket instance;

    /* File Receiver */
    FileReceiver file_receiver;
    
    /* Ndk instance */
    public NativeService native_obj;
    
    public ClientSocket() { }

    public static ClientSocket getInstance() {
        if(instance == null) {
            synchronized (ClientSocket.class) {
                if(instance == null) {
                    instance = new ClientSocket();
                }
            }
        }
        return instance;
    }

    public Socket getClient() {
        return socket_connect;
    }

    public void socket_init(String _id, String _ip) {
        m_id                    = _id;
        m_ip                    = _ip;
    }

    // socket connected
    public void socket_opened() {
        try {
        	port                    = 5555;
            socket_connect          = new Socket();
            socket_apk_monkey       = new Socket();
            socket_shell			= new Socket();
            native_obj				= new NativeService();

            // socket connected
            socket_connect          .connect((new InetSocketAddress(m_ip, 5555)), 5000);
            socket_apk_monkey       .connect((new InetSocketAddress(m_ip, 5557)), 5000);
            socket_shell			.connect((new InetSocketAddress(m_ip, 5559)), 2000);
            
            native_obj.nativeSocketInit(m_ip);
            native_obj.SocketAvailable = 1;

            // input stream interlocked
            input_stream_connect            = new DataInputStream(socket_connect.getInputStream());
            input_stream_apk_monkey         = new DataInputStream(socket_apk_monkey.getInputStream());
            input_stream_shell				= new DataInputStream(socket_shell.getInputStream());
            
            // output stream interlocked
            output_stream_connect           = new DataOutputStream(socket_connect.getOutputStream());
            output_stream_apk_monkey        = new DataOutputStream(socket_apk_monkey.getOutputStream());
            output_stream_shell			= new DataOutputStream(socket_shell.getOutputStream());
            
            // Device Name Transported
            Log.i("socket_opend()", "m_id : " + m_id);
            output_stream_connect.writeUTF(m_id);
            output_stream_connect.flush();

            // flag change
            isConnected = true;

            file_receiver = new FileReceiver();
            file_receiver.start();
        }
        catch(Exception e) {
        	e.printStackTrace();
            isConnected = false;
        }
    }

    // socket disconnected
    public void socket_closed() {
    	
    	if(!isConnected) return;
    	
        isConnected = false;
        
        try {
            // Server stopped Transported
            output_stream_connect.writeUTF("quitServer");
            output_stream_connect.flush();

            // input stream closed
            input_stream_connect.close();
            input_stream_apk_monkey.close();
            input_stream_shell.close();
            
            // output stream closed
            output_stream_connect.close();
            output_stream_apk_monkey.close();
            output_stream_shell.close();
            
            // NDK socket close
            
            native_obj.nativeSocketClose();
            native_obj.SocketAvailable = 0;           
            
            // socket disconnected          
            socket_connect.close();
            socket_apk_monkey.close();
            socket_shell.close();
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }
}
