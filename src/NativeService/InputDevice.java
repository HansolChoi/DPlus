package NativeService;

import dplus.MainActivity;
import android.app.Activity;
import android.content.Context;
import android.graphics.Point;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import Shell.ShellReceiver;

public class InputDevice{
	
	static{
		System.loadLibrary("InputService");
	}

	public native void PlayInputTest(int Width, int Height);
	// 1. 파일을 입력받는다.
	// 2. touchscreen device와 event device를 매칭시킨다.
	// 3. 해상도를 fb0 디바이스 파일에서 읽어온다. // 아니면 이전 코드(app)를 활용해도 좋다.
	// 4. 입력받은 해상도와 본 기기의 해상도에 맞춰 event device에 쓰기를 한다.
	// 5. 파일을 다 읽으면 파일을 닫는다.
	// 6. 결과 전송이 필요한지는 상의하자.
	
	public native void RecordStart(int Width, int Height);
	// 1. 파일을 생성하고(파일명은?)
	// 2. touchscreen device와 event device를 매칭시킨다.
	// 3. 해상도를 fb0 디바이스 파일에서 읽어온다. // 아니면 이전 코드(app)를 활용해도 좋다.
	// 4. event device에서 입력을 읽어온다. 이때 시간도 필요하다.(아직 시스템 시간을 어떻게 구할지 모른다.)
	// 5. 읽는 족족 파일에 쓴다. 
	
	public native void RecordStop();
	// 1. event를 읽어오는 작업을 중지하고 파일을 닫는다.
	// 2. 파일을 output_stream_apk_monkey를 통해서 전송한다. (이전 소스 참고)
	// 3. 파일 삭제가 필요한지 고민하자.
}
