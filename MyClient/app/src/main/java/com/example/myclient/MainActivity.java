package com.example.myclient;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.material.appbar.AppBarLayout;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Locale;
import java.util.Scanner;

public class MainActivity extends AppCompatActivity {

    String ip = "192.168.1.108";

    EditText terminal, et_username, et_serveraddress;
    TextView tv_feedback, tv_logged, tv_os_list;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        tv_feedback = (TextView)findViewById(R.id.tv_feedback);
        tv_os_list = (TextView)findViewById(R.id.tv_os_list);
        tv_logged = (TextView)findViewById(R.id.tv_logged);

        terminal = (EditText)findViewById(R.id.terminal);
        et_username = (EditText)findViewById(R.id.et_username);
        et_serveraddress = (EditText)findViewById(R.id.et_serveraddress);
        et_serveraddress.setText(ip);
    }

    public void send(View v){
        if(!isEditTextEmpty(et_username) || !isEditTextEmpty(et_serveraddress)) {
            MessageSender messageSender = new MessageSender();
            messageSender.execute(
                    et_username.getText().toString(),
                    et_serveraddress.getText().toString(),
                    terminal.getText().toString()
            );
        }
        else{
            tv_feedback.setText("Insert username and server address");
        }
    }

    public void refresh(View v){
        GetOsList getOsList = new GetOsList();
        getOsList.execute(
                et_username.getText().toString(),
                et_serveraddress.getText().toString()
        );
    }

    private boolean isEditTextEmpty(EditText etText) {
        return etText.getText().toString().trim().length() <= 0;
    }

    @SuppressLint("SetTextI18n")
    private boolean feedback(String msg){
        String[] lMsg = (msg.split(" "));
        tv_feedback.setText(lMsg[0].substring(0,1).toUpperCase() +
                    lMsg[0].substring(1) +
                    " " +
                    lMsg[1].replace("_", " "),
                TextView.BufferType.EDITABLE);

        return lMsg[0].contains("success");
    }

    public class MessageSender extends AsyncTask<String, Void, Void> {

        Socket s;
        PrintWriter pw;
        BufferedReader br;
        InputStream is;

        @Override
        protected Void doInBackground(String... strings) {

            String login_message = "login " + strings[0] + " 0";
            String server_address = strings[1];
            String action_message = strings[2];

            try {
                s = new Socket(server_address, 1337);
                pw = new PrintWriter(s.getOutputStream());
                is = s.getInputStream();

                pw.write(login_message);
                pw.flush();

                br = new BufferedReader(new InputStreamReader(is));
                String msgRec = br.readLine();
                if (feedback(msgRec)){
                    if(!action_message.isEmpty()){ // send second msg only if exists
                        pw.write(action_message);
                        pw.flush();
                        msgRec = br.readLine();
                        if (feedback(msgRec)){ // if feedback success clear terminal
                            terminal.setText("");
                        }
                    }
                }
                br.close();
                pw.close();
                s.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
            return null;
        }
    }

    public class GetOsList extends AsyncTask<String, Void, Void> {

        Socket s;
        PrintWriter pw;
        BufferedReader br;
        InputStream is;

        @Override
        protected Void doInBackground(String... strings) {

            String ask_msg = "get_active_os " + strings[0] + " 0";
            String server_address = strings[1];

            try {
                s = new Socket(server_address, 1337);
                pw = new PrintWriter(s.getOutputStream());
                is = s.getInputStream();

                pw.write(ask_msg);
                pw.flush();

                br = new BufferedReader(new InputStreamReader(is));
                String msgRec = br.readLine();

                Integer os_number = Integer.valueOf(msgRec.split(" ")[1]);
                ArrayList<String> os_list = new ArrayList<String>();
                for (int i = 0; i<os_number; i++){
                    os_list.add(br.readLine());
                }
                tv_os_list.setText(TextUtils.join("\n", os_list));

                br.close();
                pw.close();
                s.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
            return null;
        }
    }
}