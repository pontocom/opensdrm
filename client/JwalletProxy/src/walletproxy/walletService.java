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

import java.util.*;
import java.net.*;
import java.io.*;

import javax.net.ssl.*;
import com.sun.net.ssl.internal.www.protocol.https.*;

import java.security.Security;

import java.security.interfaces.*;


public class walletService
{
    public String requestWALLETgetKey(String uid, String cid, String rlvl, String sURL)
    {
        try {
            String request =
                "uid=" + URLEncoder.encode(uid) +
                "&cid=" + URLEncoder.encode(cid) +
                "&rlvl=" + URLEncoder.encode(rlvl);

            System.out.println("HTTP Request = "+request);

            //URL wallet_url = new URL("http://192.168.0.221/opensdrm/osdrmWBwallet/Wallet.ws.php" + "?" + request);
            URL wallet_url = new URL(sURL + "?" + request);

            System.out.println(wallet_url);

            HttpURLConnection wallet_conn = (HttpURLConnection)(wallet_url.openConnection());
            wallet_conn.setRequestMethod("GET");
            wallet_conn.setRequestProperty("Content-Type", "*/*");
            wallet_conn.setDoInput(true);
            wallet_conn.connect();

            //int len = dumpResponse(wallet_conn);

            BufferedReader in = new BufferedReader(new InputStreamReader(wallet_conn.getInputStream()));

            //InputStream in = wallet_conn.getInputStream();

            String res = in.readLine();

            if(res.compareTo("<br />")==0) return "ERROR";

            //System.out.println("Result received is:"+res);

            in.close();
            wallet_conn.disconnect();

            return res;

        } catch (Exception e) {
                System.out.println("AUSrequestUserSubscription (L1)" + "\t Exception: " + e);
                e.printStackTrace();
                return "ERROR";
        }
    }

    public String requestSecuredWALLETgetKey(String uid, String cid, String rlvl, String sURL)
    {
        try {
            /* Parse the temporary URL */
            URL tmpUrl = new URL(sURL);
            String host = tmpUrl.getHost();

            Security.addProvider(new com.sun.net.ssl.internal.ssl.Provider());
            SSLSocketFactory sfactory = (SSLSocketFactory)SSLSocketFactory.getDefault();
            //SSLSocket ssocket = (SSLSocket) sfactory.createSocket("127.0.0.1", 443);
            SSLSocket ssocket = (SSLSocket) sfactory.createSocket(host, 443);

            ssocket.startHandshake();

            PrintWriter out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(ssocket.getOutputStream())));

            String request =
                "uid=" + URLEncoder.encode(uid) +
                "&cid=" + URLEncoder.encode(cid) +
                "&rlvl=" + URLEncoder.encode(rlvl);


            out.println("GET " + sURL + "?" + request);
            System.out.println("GET " + sURL + "?" + request);
            out.println();
            out.flush();

            /*
            * Make sure there were no surprises
            */
            if (out.checkError())
              System.out.println("SSLSocketClient:  java.io.PrintWriter error");

            /* read response */
            BufferedReader in = new BufferedReader(new InputStreamReader(ssocket.getInputStream()));

            String res = in.readLine();
            /*String res;
            do{
                res = in.readLine();
                System.out.println("KEY = "+res);
            } while (res.compareTo("")!=0);*/

            if(res.compareTo("<br />")==0) return "ERROR";

            //System.out.println("Result received is:"+res);

            in.close();
            out.close();
            ssocket.close();

            return res;
        } catch (Exception e) {
                System.out.println("AUSrequestUserSubscription (L1)" + "\t Exception: " + e);
                e.printStackTrace();
                return "ERROR";
        }
    }


    public walletService()
    {
    }
}