package com.example.androidclient;

import android.os.AsyncTask;

import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;

public class MessageSender extends AsyncTask<String, Void, Void> {
    private Socket s;
    private PrintWriter pw;

    @Override
    protected Void doInBackground(String... voids) {

        String serverAddress = voids[0];
        String name = voids[1];
        String msg = voids[2];
        try {
            s = new Socket(serverAddress, 1337);

            pw = new PrintWriter(s.getOutputStream());
            pw.write(msg + " " + name + " 0");
            pw.flush();
            pw.close();
            s.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }
}
