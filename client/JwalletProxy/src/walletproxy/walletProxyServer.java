// A server that uses multithreading
// to handle any number of clients.
/*
 OpenSDRM - Open Secure Digital Rights Management
 Copyright (c) 2002 Adetti

 This software was originaly developed under the MOSES project (IST-2001-34144)

________                         _________________ __________    _____
\_____  \ ______   ____   ____  /   _____/\______ \\______   \  /     \
 /   |   \\____ \_/ __ \ /    \ \_____  \  |    |  \|       _/ /  \ /  \
/    |    \  |_> >  ___/|   |  \/        \ |    `   \    |   \/    Y    \
\_______  /   __/ \___  >___|  /_______  //_______  /____|_  /\____|__  /
        \/|__|        \/     \/        \/         \/       \/         \/

This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 2.1 of the License, or (at your option)
any later version.

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
details.

You should have received a copy of the GNU Lesser General Public License along
with this library; if not, write to the Free Software Foundation, Inc., 59
Temple Place, Suite 330, Boston, MA 02111-1307 USA


*/
package walletproxy;


import java.io.*;
import java.net.*;

class walletProxyHandler extends Thread {
  private Socket socket;
  private BufferedReader in;
  private PrintWriter out;
  public String sURL;
  public String sUid;

  public walletProxyHandler(Socket s)
      throws IOException {
    socket = s;
    in =
      new BufferedReader(
        new InputStreamReader(
          socket.getInputStream()));
    // Enable auto-flush:
    out =
      new PrintWriter(
        new BufferedWriter(
          new OutputStreamWriter(
            socket.getOutputStream())), true);
    // If any of the above calls throw an
    // exception, the caller is responsible for
    // closing the socket. Otherwise the thread
    // will close it.
    start(); // Calls run()
  }

  public void readCFFile() {
    sURL = new String();
    sUid = new String();
    try {
        BufferedReader in = new BufferedReader(new FileReader("client.info"));
        sURL = in.readLine();
        sUid = in.readLine();
        in.close();
    } catch (IOException e) {
        System.out.println("Couldn't open the file!");
    }
  }

  public void run() {
      readCFFile();
    try {
      while (true) {
        String str = in.readLine();
        //System.out.println(str);
        /*if (str == null) {
            continue;
        }*/
        if (str != null) {
            if (str.equals("quit")) {
                break;
            } else if (str.length() < 6) {
                out.println("ERROR");
            } else if ((str.substring(0,6)).equals("GETKEY")) {
                // we have to parse the request and ask for the key through the WS
                String params[] = str.split(" ");
                //System.out.println(params[0]);
                //System.out.println(params[1]); // level
                //System.out.println(params[2]); // cid
                //System.out.println(params[3]); // uid
                //str.spl

                // workaround to avoid garbage at the end of the CID
                String cid = params[2].substring(0, 45);
                //String cid="";

                walletService ws = new walletService();

                String key = "";

                if(sURL.startsWith("http://")) {
                    System.out.println("HTTP");
                    key = ws.requestWALLETgetKey(sUid, cid, params[1], sURL);
                } else if (sURL.startsWith("https://")) {
                    System.out.println("HTTPS");
                    key = ws.requestSecuredWALLETgetKey(sUid, cid, params[1], sURL);
                }
                //key = ws.requestSecuredWALLETgetKey("00400001", "urn:mpeg:mpeg21:diid:doi:int/esa/eop/00000001", "0", sURL);

                out.println(key);
            } else {
                out.println("ERROR");
            }
        }
      }
      System.out.println("closing...");
    } catch(IOException e) {
      System.err.println("IO Exception");
    } finally {
      try {
        socket.close();
      } catch(IOException e) {
        System.err.println("Socket not closed");
      }
    }
  }
}

public class walletProxyServer {
  static final int PORT = 8001;
  public static void main(String[] args)
      throws IOException {
    ServerSocket s = new ServerSocket(PORT);
    System.out.println("Server Started");
    try {
      while(true) {
        // Blocks until a connection occurs:
        Socket socket = s.accept();
        try {
          new walletProxyHandler(socket);
        } catch(IOException e) {
          // If it fails, close the socket,
          // otherwise the thread will close it:
          socket.close();
        }
      }
    } finally {
      s.close();
    }
  }
} ///:~
