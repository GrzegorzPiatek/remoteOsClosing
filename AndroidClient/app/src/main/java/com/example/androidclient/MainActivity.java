package com.example.androidclient;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.Spinner;

import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {

    private Spinner osSpinner;
    private Messenger msgSender = getIntent().getParcelableExtra("messenger");
    private ArrayList<String> osNames;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

}