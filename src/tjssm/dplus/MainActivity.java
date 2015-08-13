package tjssm.dplus;

import android.app.Activity;
import android.os.Bundle;
import Socket.Socket;

public class MainActivity extends Activity {
	Socket socket;
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		socket.Create();

		socket.Stop();
	}
}