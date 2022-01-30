package com.company;

import java.io.*;
import java.net.Socket;

class Agent{
    private Socket s;

    private PrintWriter pw;
    private BufferedReader br;
    private InputStream is;

    String address;
    String SystemName;
    String permission_lvl;

    public void start() {
        String login_message = "new_os " + SystemName + " " + permission_lvl + "\n";

        try {
            s = new Socket(address, 1337);
            pw = new PrintWriter(s.getOutputStream());
            is = s.getInputStream();

            pw.write(login_message);
            pw.flush();

            br = new BufferedReader(new InputStreamReader(is));
            String msgRec = br.readLine();
            while(!msgRec.contains("close_os"))
                msgRec = br.readLine();
            br.close();
            pw.close();
            s.close();
            shutdown();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void shutdown() throws RuntimeException, IOException {
        String shutdownCommand;
//        shutdownCommand = "shutdown -h now"; // unix + mac
        shutdownCommand = "shutdown.exe -s -t 0"; // windows
        Runtime.getRuntime().exec(shutdownCommand);
        System.exit(0);
    }
}