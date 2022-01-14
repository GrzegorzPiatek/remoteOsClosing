package com.example.androidclient;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.EditText;

public class MainActivity extends AppCompatActivity {

    EditText terminal;
    Messenger msgSender = getIntent().getParcelableExtra("messenger");

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        terminal = (EditText)findViewById(R.id.terminal);
    }

    public void send(View v){
        // msgSender.execute(terminal.getText().toString());
    }
}