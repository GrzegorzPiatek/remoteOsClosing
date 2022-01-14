package com.example.androidclient;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import java.net.Socket;
import java.util.concurrent.ExecutionException;

public class LoginActivity extends AppCompatActivity {

    private EditText eUsername;
    private EditText eServerAddress;
    private Button eLogin;
    private TextView eLoginInfo;

    private String username;

    private boolean isValid;

    public Socket s;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        eUsername = findViewById(R.id.loginTextUsername);
        eServerAddress = findViewById(R.id.loginTextServerAddress);
        eLogin = findViewById(R.id.loginButton);
        eLoginInfo = findViewById(R.id.loginInfo);

        eLogin.setOnClickListener(new View.OnClickListener() {
            @SuppressLint("SetTextI18n")
            @Override
            public void onClick(View view) {
                String inputName = eUsername.getText().toString();
                String inputServerAddress = eServerAddress.getText().toString();

                if(inputName.isEmpty() || inputServerAddress.isEmpty()){
                    eLoginInfo.setText("Please fill all fields.");
                }
                else if(inputName.contains(" ") || inputServerAddress.contains(" ")){
                    eLoginInfo.setText("Spaces are not allowed.");
                }
                else{
                    isValid = login(inputServerAddress, inputName);
                    if(!isValid){
                        eLoginInfo.setText("Problem with login!");
                    }
                    else{
                        Intent intent = new Intent(LoginActivity.this, MainActivity.class);
                        intent.putExtra("username", username);
                        startActivity(intent);
                    }
                }
            }
        });
    }

    private boolean login(String serverAddress, String username){
        Messenger msgSnd = new Messenger();
        msgSnd.execute(serverAddress, username, "login");
        try {
            return msgSnd.get().contains("success");
        } catch (ExecutionException | InterruptedException e) {
            return false;
        }
    }
}
