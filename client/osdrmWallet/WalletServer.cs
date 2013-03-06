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
using System;
using System.Net.Sockets;
using System.Net;
using System.Threading;
using System.IO;
using System.Text;
using System.Windows.Forms;

namespace osdrmWallet
{
	/// <summary>
	/// Summary description for WalletServer.
	/// </summary>
	///

	public class WalletServer
	{
		public const int PORT = 99999;
		public TcpListener ligServ;
		public bool m_shouldrun = true; // this variable controls, either if the server should run or not
		public WalletClientHdlr cli;


		public WalletServer(int port, string uname, string pwd)
		{
			//
			// TODO: Add constructor logic here
			//
			ligServ = null;
			try
			{
				//ligServ = new TcpListener(port);
				ligServ = new TcpListener(System.Net.IPAddress.Parse("127.0.0.1"), port);
				ligServ.Start();

				while(m_shouldrun==true)
				{
					//MessageBox.Show("antes do accept");
					TcpClient ligCli = ligServ.AcceptTcpClient();
					//MessageBox.Show("depois do accept");
					cli = new WalletClientHdlr(ligCli, uname, pwd);
					//MessageBox.Show("depois do handler");
				}
			}
			catch (ThreadAbortException)
			{
				//System.Windows.Forms.MessageBox.Show("estou aqui: ThreadAbortException...");
				cli.killHandler();
			}
			catch(ThreadInterruptedException)
			{
				//System.Windows.Forms.MessageBox.Show("estou aqui: : ThreadInterruptedException...");
				cli.killHandler();
			}
			catch (Exception e)
			{
				System.Windows.Forms.MessageBox.Show(e.Message);
				System.Windows.Forms.MessageBox.Show(e.StackTrace);
			}
			finally
			{
				if(ligServ!=null)
					ligServ.Stop();
			}
		}

		public void killServer()
		{
			m_shouldrun = false;
			ligServ.Stop();
		}
	}
}
