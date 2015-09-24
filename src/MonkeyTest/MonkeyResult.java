package MonkeyTest;

import Socket.ClientSocket;

import java.io.BufferedReader;
import java.io.InputStreamReader;

/**
 * Created by namhoon on 2015-06-19.
 */
public class MonkeyResult implements Runnable {
    InputStreamReader   input_stream_reader;
    BufferedReader      buffered_reader;

    public MonkeyResult(InputStreamReader _input_stream_reader) {
        input_stream_reader = _input_stream_reader;
        buffered_reader     = new BufferedReader(_input_stream_reader);
    }

    @Override
    public void run() {
        try {
            ClientSocket.output_stream_apk_monkey.writeUTF("-SendMonkeyTestResult-");
            //KernelService.toast("결과를 전송합니다.");
        }
        catch(Exception e) { e.printStackTrace(); }
        String line = null;
        try {
            while( (line = buffered_reader.readLine()) != null) {
                ClientSocket.output_stream_apk_monkey.writeUTF(line);
                ClientSocket.output_stream_apk_monkey.flush();
            }
            ClientSocket.output_stream_apk_monkey.writeUTF("-end-");
            ClientSocket.output_stream_apk_monkey.flush();
            input_stream_reader.close();
        }
        catch(Exception e) { e.printStackTrace(); }
    }
}