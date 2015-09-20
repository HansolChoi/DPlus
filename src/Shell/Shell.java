package Shell;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;

public class Shell {
	public OutputStream compileOutputStream;
    public Process compileProcess;
    public PrintWriter stdin;
    
    class SyncPipe implements Runnable
    {
        public SyncPipe(InputStream istrm, OutputStream ostrm) {
            istrm_ = istrm;
            ostrm_ = ostrm;
        }
        public void run() {
            try
            {
                final byte[] buffer = new byte[1024];
                for (int length = 0; (length = istrm_.read(buffer)) != -1; )
                {
                    ostrm_.write(buffer, 0, length);
                }
            }
            catch (Exception e)
            {
                e.printStackTrace();
            }
        }
        private final OutputStream ostrm_;
        private final InputStream istrm_;
    }

    public void setStream(){
        compileProcess = null;

        try {
            compileProcess = Runtime.getRuntime().exec("/system/bin/sh");
        } catch (IOException e1) {
            e1.printStackTrace();
        }

        compileOutputStream = new OutputStream() {
            @Override
            public void write(int b) throws IOException {
                char c = (char) b;
                String res = new String(String.valueOf(c));

            }

            @Override
            public void write(byte[] b, int off, int len) throws IOException {               
                super.write(b, off, len);
            }
        };

        new Thread(new SyncPipe(compileProcess.getInputStream(), compileOutputStream)).start();
        stdin = new PrintWriter(compileProcess.getOutputStream());
   
    }
}
