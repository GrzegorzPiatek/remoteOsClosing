package com.example.androidclient;

import android.os.AsyncTask;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

public class Messenger extends AsyncTask<String, Void, String> {
    private Socket s;
    private PrintWriter pw;
    private InputStreamReader isr;
    private BufferedReader buffReader;
    private String recMsg = "";

    @Override
    protected String doInBackground(String... voids) {

        String serverAddress = voids[0];
        String name = voids[1];
        String msg = voids[2];
        try {
            s = new Socket(serverAddress, 1337);

            pw = new PrintWriter(s.getOutputStream(), true);
            pw.write(msg + " " + name + " 0\n");

            isr = new InputStreamReader(s.getInputStream());
            buffReader = new BufferedReader(isr);
            recMsg = buffReader.readLine();
            isr.close();
            pw.flush();
            pw.close();
            s.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return recMsg;
    }

    public String getRecMsg() {
        return recMsg;
    }
}

