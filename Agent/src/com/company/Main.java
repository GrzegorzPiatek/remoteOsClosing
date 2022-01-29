package com.company;

import java.net.*;

public class Main {

    public static void main(String[] args) {
        Agent agent = new Agent();
        if (args.length >= 2) {
            agent.address = args[0];
            agent.permission_lvl = args[1];
            try {
                agent.SystemName = InetAddress.getLocalHost().getHostName();
            } catch (UnknownHostException e) {
                e.printStackTrace();
            }
            agent.start();
        }
        else{
            System.out.println("Give 2 params: 1. host address 2. minimum permission to close this OS\n");
            System.exit(2);
        }
    }
}
