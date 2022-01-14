package com.example.androidclient;

import android.os.AsyncTask;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;

public class MessageReceiver extends AsyncTask<String, Void, String> {

    Socket s;
    InputStreamReader isr;
    BufferedReader buffReader;

    public void createSocket(String address){
        try {
            s = new Socket(address, 1337);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void closeSocket(){
        try {
            s.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    protected String doInBackground(String... voids) {

        String message = voids[0];

        try {
            isr = new InputStreamReader(s.getInputStream());
            buffReader = new BufferedReader(isr);
            message = buffReader.readLine();
            isr.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return message;
    }

}
