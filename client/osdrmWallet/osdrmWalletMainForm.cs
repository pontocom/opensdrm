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
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Threading;
using System.Net.Sockets;
using System.Management;
using System.Text;
using Microsoft.Win32;

namespace osdrmWallet
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class osdrmWltMainForm : System.Windows.Forms.Form
	{
		public System.Windows.Forms.Button startWalletBtn;
		private System.Windows.Forms.Label statusLabel;
		public System.Windows.Forms.Button registerUserBtn;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		public System.Windows.Forms.TextBox usernameTxt;
		public System.Windows.Forms.TextBox passwordTxt;

		private System.Windows.Forms.Form registrationForm;

		private static osdrmWltMainForm mf;

		public Thread goSrv; // This holds the handler for the server part;
		public WalletServer ws; // an handler to the server execution part;

		private string uname, pwd;

		public bool is_running;
		private System.Windows.Forms.NotifyIcon notifyIcon1;
		private System.Windows.Forms.ContextMenu contextMenu1;
		private System.Windows.Forms.MenuItem menuItem1;
		private System.Windows.Forms.MenuItem menuItem2;
		private System.Windows.Forms.MenuItem menuItem3;
		private System.Windows.Forms.MenuItem menuItem4;
		private System.Windows.Forms.MenuItem menuItem5;
		private System.Windows.Forms.MenuItem menuItem6;
		private System.Windows.Forms.MenuItem menuItem7;
		private System.Windows.Forms.PictureBox pictureBox1;
		private System.ComponentModel.IContainer components;

		public osdrmWltMainForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//

			RegistryKey rk = Registry.CurrentUser.CreateSubKey(@"Software\OpenSDRMWallet");
			//rk.CreateSubKey(@"licenses");
			rk.Close();

			//RegistryKey rkey = null;
			//rkey = Registry.CurrentUser.OpenSubKey(@"Software\OpenSDRMWallet", true);
			//key.SetValue("teste", "123");
			//rkey.Close();

			// Validate if the User has or not yet the data files created
			// if yes, the buttons and textboxes are disabled and only registration of user is possible
			// else disable the registration button
			CUser user = new CUser();
			//menuItem1.Enabled=false;
			if(user.checkDataFiles()==false) // disable buttons
			{
				usernameTxt.Enabled = false;
				passwordTxt.Enabled = false;
				startWalletBtn.Enabled = false;
				registerUserBtn.Enabled = true;

				menuItem5.Enabled=true;
			}
			else
			{
				usernameTxt.Enabled = true;
				passwordTxt.Enabled = true;
				startWalletBtn.Enabled = true;
				registerUserBtn.Enabled = false;

				menuItem5.Enabled=false;
			}

			is_running = false;
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(osdrmWltMainForm));
			this.startWalletBtn = new System.Windows.Forms.Button();
			this.statusLabel = new System.Windows.Forms.Label();
			this.registerUserBtn = new System.Windows.Forms.Button();
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.usernameTxt = new System.Windows.Forms.TextBox();
			this.passwordTxt = new System.Windows.Forms.TextBox();
			this.notifyIcon1 = new System.Windows.Forms.NotifyIcon(this.components);
			this.contextMenu1 = new System.Windows.Forms.ContextMenu();
			this.menuItem2 = new System.Windows.Forms.MenuItem();
			this.menuItem3 = new System.Windows.Forms.MenuItem();
			this.menuItem5 = new System.Windows.Forms.MenuItem();
			this.menuItem4 = new System.Windows.Forms.MenuItem();
			this.menuItem6 = new System.Windows.Forms.MenuItem();
			this.menuItem7 = new System.Windows.Forms.MenuItem();
			this.menuItem1 = new System.Windows.Forms.MenuItem();
			this.pictureBox1 = new System.Windows.Forms.PictureBox();
			this.SuspendLayout();
			//
			// startWalletBtn
			//
			this.startWalletBtn.Location = new System.Drawing.Point(152, 144);
			this.startWalletBtn.Name = "startWalletBtn";
			this.startWalletBtn.TabIndex = 0;
			this.startWalletBtn.Text = "Start Wallet";
			this.startWalletBtn.Click += new System.EventHandler(this.startWalletBtn_Click);
			//
			// statusLabel
			//
			this.statusLabel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.statusLabel.Location = new System.Drawing.Point(8, 208);
			this.statusLabel.Name = "statusLabel";
			this.statusLabel.Size = new System.Drawing.Size(272, 16);
			this.statusLabel.TabIndex = 1;
			this.statusLabel.Text = "Wallet is stopped...";
			//
			// registerUserBtn
			//
			this.registerUserBtn.Location = new System.Drawing.Point(112, 168);
			this.registerUserBtn.Name = "registerUserBtn";
			this.registerUserBtn.Size = new System.Drawing.Size(112, 23);
			this.registerUserBtn.TabIndex = 2;
			this.registerUserBtn.Text = "Register New User";
			this.registerUserBtn.Click += new System.EventHandler(this.registerUserBtn_Click);
			//
			// label1
			//
			this.label1.Location = new System.Drawing.Point(48, 88);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(72, 16);
			this.label1.TabIndex = 3;
			this.label1.Text = "Username:";
			//
			// label2
			//
			this.label2.Location = new System.Drawing.Point(48, 112);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(72, 16);
			this.label2.TabIndex = 4;
			this.label2.Text = "Password:";
			//
			// usernameTxt
			//
			this.usernameTxt.Location = new System.Drawing.Point(112, 88);
			this.usernameTxt.Name = "usernameTxt";
			this.usernameTxt.Size = new System.Drawing.Size(120, 20);
			this.usernameTxt.TabIndex = 5;
			this.usernameTxt.Text = "";
			//
			// passwordTxt
			//
			this.passwordTxt.Location = new System.Drawing.Point(112, 112);
			this.passwordTxt.Name = "passwordTxt";
			this.passwordTxt.PasswordChar = '*';
			this.passwordTxt.Size = new System.Drawing.Size(120, 20);
			this.passwordTxt.TabIndex = 6;
			this.passwordTxt.Text = "";
			//
			// notifyIcon1
			//
			this.notifyIcon1.ContextMenu = this.contextMenu1;
			this.notifyIcon1.Icon = ((System.Drawing.Icon)(resources.GetObject("notifyIcon1.Icon")));
			this.notifyIcon1.Text = "OSDRM Wallet";
			this.notifyIcon1.Visible = true;
			this.notifyIcon1.DoubleClick += new System.EventHandler(this.notifyIcon1_DoubleClick);
			//
			// contextMenu1
			//
			this.contextMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																						 this.menuItem2,
																						 this.menuItem3,
																						 this.menuItem5,
																						 this.menuItem4,
																						 this.menuItem6,
																						 this.menuItem7,
																						 this.menuItem1});
			//
			// menuItem2
			//
			this.menuItem2.Index = 0;
			this.menuItem2.Text = "Open";
			this.menuItem2.Click += new System.EventHandler(this.menuItem2_Click);
			//
			// menuItem3
			//
			this.menuItem3.Index = 1;
			this.menuItem3.Text = "-";
			//
			// menuItem5
			//
			this.menuItem5.Index = 2;
			this.menuItem5.Text = "Register New...";
			this.menuItem5.Click += new System.EventHandler(this.menuItem5_Click);
			//
			// menuItem4
			//
			this.menuItem4.Index = 3;
			this.menuItem4.Text = "Registration Info";
			this.menuItem4.Click += new System.EventHandler(this.menuItem4_Click);
			//
			// menuItem6
			//
			this.menuItem6.Index = 4;
			this.menuItem6.Text = "-";
			//
			// menuItem7
			//
			this.menuItem7.Index = 5;
			this.menuItem7.Text = "Start Wallet";
			this.menuItem7.Click += new System.EventHandler(this.menuItem7_Click);
			//
			// menuItem1
			//
			this.menuItem1.Index = 6;
			this.menuItem1.Text = "Terminate Wallet";
			this.menuItem1.Click += new System.EventHandler(this.menuItem1_Click);
			//
			// pictureBox1
			//
			this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
			this.pictureBox1.Location = new System.Drawing.Point(4, 0);
			this.pictureBox1.Name = "pictureBox1";
			this.pictureBox1.Size = new System.Drawing.Size(296, 80);
			this.pictureBox1.TabIndex = 7;
			this.pictureBox1.TabStop = false;
			//
			// osdrmWltMainForm
			//
			this.AutoScale = false;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.BackColor = System.Drawing.SystemColors.Control;
			this.ClientSize = new System.Drawing.Size(304, 230);
			this.Controls.Add(this.pictureBox1);
			this.Controls.Add(this.passwordTxt);
			this.Controls.Add(this.usernameTxt);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.registerUserBtn);
			this.Controls.Add(this.statusLabel);
			this.Controls.Add(this.startWalletBtn);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.Name = "osdrmWltMainForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "osdrm Wallet";
			this.Resize += new System.EventHandler(this.osdrmWltMainForm_Resize);
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main()
		{
			mf = new osdrmWltMainForm();
			//Application.Run(new osdrmWltMainForm());
			Application.Run(mf);
		}

		private void registerUserBtn_Click(object sender, System.EventArgs e)
		{
			registrationForm = new userRegistrationForm(mf);
			registrationForm.ShowDialog();
		}

		private void startWalletBtn_Click(object sender, System.EventArgs e)
		{
			if(is_running == false)
			{
				CUser user = new CUser();
				if(user.validateLogin(usernameTxt.Text, passwordTxt.Text))
				{
					//MessageBox.Show("Username is valid!!!");

					uname = usernameTxt.Text;
					pwd = passwordTxt.Text;

					usernameTxt.Text = "";
					usernameTxt.Enabled = false;
					passwordTxt.Text = "";
					passwordTxt.Enabled = false;
					startWalletBtn.Text = "Terminate Wallet!!!";

					//menuItem1.Enabled = true;
					menuItem7.Enabled = false;
					menuItem5.Enabled = false;

					statusLabel.Text = "Wallet server is running...";
					notifyIcon1.Text = "OSDRM Wallet - Wallet server is running...";

					is_running = true;

					goSrv = new Thread(new ThreadStart(this.goServer));
					goSrv.IsBackground = true;
					goSrv.Start();

					WindowState = FormWindowState.Minimized;
				}
				else
				{
					MessageBox.Show("Username is invalid!!!");
				}
			}
			else if(is_running == true)
			{
				//if(goSrv.IsAlive)
				//MessageBox.Show("estou a tentar matar a thread");
				goSrv.Interrupt();
				goSrv.Abort();

				//TcpClient cli = new TcpClient("127.0.0.1", 9999);
				//cli.Close();

				//this.stopServer();
				//goSrv.Abort();

				this.Dispose();
				Application.Exit();
			}
		}

		private void goServer()
		{
			ws = new WalletServer(8001, uname, pwd);
		}

		private void stopServer()
		{
			ws.killServer();
		}

		private void osdrmWltMainForm_Resize(object sender, System.EventArgs e)
		{
			if(FormWindowState.Minimized == WindowState)
			{
				Hide();
			}
		}

		private void notifyIcon1_DoubleClick(object sender, System.EventArgs e)
		{
			Show();
			WindowState = FormWindowState.Normal;
		}

		private void menuItem1_Click(object sender, System.EventArgs e)
		{
			if(goSrv!=null)
			{
				goSrv.Interrupt();
				goSrv.Abort();
			}

			//TcpClient cli = new TcpClient("127.0.0.1", 9999);
			//cli.Close();

			//this.stopServer();
			//goSrv.Abort();

			this.Dispose();
			Application.Exit();
		}

		private void menuItem2_Click(object sender, System.EventArgs e)
		{
			Show();
			WindowState = FormWindowState.Normal;
		}

		private void menuItem7_Click(object sender, System.EventArgs e)
		{
			Show();
			WindowState = FormWindowState.Normal;
		}

		private void menuItem5_Click(object sender, System.EventArgs e)
		{
			registrationForm = new userRegistrationForm(mf);
			registrationForm.ShowDialog();
		}

		private void menuItem4_Click(object sender, System.EventArgs e)
		{
			/*CUser cu = new CUser();
			string cifrados = cu.cipherDataS("xpto", "cjcs", "dados que vao ser cifrados");
			MessageBox.Show(cifrados);

			string decifrados = cu.decipherDataS("xpto", "cjcs", cifrados);
			MessageBox.Show(decifrados);*/

			/*RSAEncDec rsa = new RSAEncDec();
			rsa.RSAcreatekeys(1024);

			MessageBox.Show(rsa.returnPublicComponents());
			MessageBox.Show(rsa.returnPrivateComponents());*/

			/*string lic = null;
			osdrmh2kLIS1.opensdrmws ws = new osdrmWallet.osdrmh2kLIS1.opensdrmws();
			ws.LISrequestLicenseDownload("00010001", "urn:mpeg:mpeg21:diid:doi:pt/adetti/music/00000004", "<auth><username>f</username><password>f</password></auth>", out lic);

			MessageBox.Show(lic);*/

			RegistryKey rk = Registry.LocalMachine.OpenSubKey("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion");
			string pname = rk.GetValue("ProductName").ToString();

		}


	}
}
