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
	// 1. ������ �Է¹޴´�.
	// 2. touchscreen device�� event device�� ��Ī��Ų��.
	// 3. �ػ󵵸� fb0 ����̽� ���Ͽ��� �о�´�. // �ƴϸ� ���� �ڵ�(app)�� Ȱ���ص� ����.
	// 4. �Է¹��� �ػ󵵿� �� ����� �ػ󵵿� ���� event device�� ���⸦ �Ѵ�.
	// 5. ������ �� ������ ������ �ݴ´�.
	// 6. ��� ������ �ʿ������� ��������.
	
	public native void RecordStart(int Width, int Height);
	// 1. ������ �����ϰ�(���ϸ���?)
	// 2. touchscreen device�� event device�� ��Ī��Ų��.
	// 3. �ػ󵵸� fb0 ����̽� ���Ͽ��� �о�´�. // �ƴϸ� ���� �ڵ�(app)�� Ȱ���ص� ����.
	// 4. event device���� �Է��� �о�´�. �̶� �ð��� �ʿ��ϴ�.(���� �ý��� �ð��� ��� ������ �𸥴�.)
	// 5. �д� ���� ���Ͽ� ����. 
	
	public native void RecordStop();
	// 1. event�� �о���� �۾��� �����ϰ� ������ �ݴ´�.
	// 2. ������ output_stream_apk_monkey�� ���ؼ� �����Ѵ�. (���� �ҽ� ����)
	// 3. ���� ������ �ʿ����� �������.
}
