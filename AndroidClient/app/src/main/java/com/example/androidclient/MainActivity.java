package com.example.androidclient;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.text.Editable;
import android.view.View;
import android.widget.EditText;

public class MainActivity extends AppCompatActivity {

    EditText terminal;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        terminal = (EditText)findViewById(R.id.terminal);
    }

    public void send(View v){
        MessageSender messageSender = new MessageSender();
        messageSender.execute(terminal.getText().toString());
    }
}