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

namespace osdrmWallet
{
	/// <summary>
	/// Summary description for userRegistrationForm.
	/// </summary>
	public class userRegistrationForm : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Button registerBtn;
		private System.Windows.Forms.Button cancelBtn;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.TextBox usernameTxt;
		private System.Windows.Forms.TextBox passwordTxt;
		private System.Windows.Forms.TextBox repasswordTxt;
		private System.Windows.Forms.TextBox renameTxt;
		private System.Windows.Forms.TextBox addressTxt;
		private System.Windows.Forms.TextBox emailTxt;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		private osdrmWltMainForm mf;

		public userRegistrationForm(osdrmWltMainForm mf)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//

			this.mf = mf;
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
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
			this.registerBtn = new System.Windows.Forms.Button();
			this.cancelBtn = new System.Windows.Forms.Button();
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.label6 = new System.Windows.Forms.Label();
			this.label7 = new System.Windows.Forms.Label();
			this.usernameTxt = new System.Windows.Forms.TextBox();
			this.passwordTxt = new System.Windows.Forms.TextBox();
			this.repasswordTxt = new System.Windows.Forms.TextBox();
			this.renameTxt = new System.Windows.Forms.TextBox();
			this.addressTxt = new System.Windows.Forms.TextBox();
			this.emailTxt = new System.Windows.Forms.TextBox();
			this.SuspendLayout();
			//
			// registerBtn
			//
			this.registerBtn.Location = new System.Drawing.Point(208, 232);
			this.registerBtn.Name = "registerBtn";
			this.registerBtn.TabIndex = 0;
			this.registerBtn.Text = "Register";
			this.registerBtn.Click += new System.EventHandler(this.registerBtn_Click);
			//
			// cancelBtn
			//
			this.cancelBtn.Location = new System.Drawing.Point(120, 232);
			this.cancelBtn.Name = "cancelBtn";
			this.cancelBtn.TabIndex = 1;
			this.cancelBtn.Text = "Cancel";
			this.cancelBtn.Click += new System.EventHandler(this.cancelBtn_Click);
			//
			// label1
			//
			this.label1.Location = new System.Drawing.Point(0, 8);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(288, 32);
			this.label1.TabIndex = 2;
			this.label1.Text = "Please fill-in all the fields and then press the REGISTER button to process the r" +
				"egistration:";
			//
			// label2
			//
			this.label2.Location = new System.Drawing.Point(8, 48);
			this.label2.Name = "label2";
			this.label2.TabIndex = 3;
			this.label2.Text = "Username:";
			//
			// label3
			//
			this.label3.Location = new System.Drawing.Point(8, 72);
			this.label3.Name = "label3";
			this.label3.TabIndex = 4;
			this.label3.Text = "Password:";
			//
			// label4
			//
			this.label4.Location = new System.Drawing.Point(8, 96);
			this.label4.Name = "label4";
			this.label4.TabIndex = 5;
			this.label4.Text = "Re-type password:";
			//
			// label5
			//
			this.label5.Location = new System.Drawing.Point(8, 122);
			this.label5.Name = "label5";
			this.label5.TabIndex = 6;
			this.label5.Text = "Real name:";
			//
			// label6
			//
			this.label6.Location = new System.Drawing.Point(8, 152);
			this.label6.Name = "label6";
			this.label6.TabIndex = 7;
			this.label6.Text = "Adress:";
			//
			// label7
			//
			this.label7.Location = new System.Drawing.Point(8, 200);
			this.label7.Name = "label7";
			this.label7.TabIndex = 8;
			this.label7.Text = "Email:";
			//
			// usernameTxt
			//
			this.usernameTxt.Location = new System.Drawing.Point(112, 48);
			this.usernameTxt.Name = "usernameTxt";
			this.usernameTxt.Size = new System.Drawing.Size(160, 20);
			this.usernameTxt.TabIndex = 9;
			this.usernameTxt.Text = "";
			//
			// passwordTxt
			//
			this.passwordTxt.Location = new System.Drawing.Point(112, 72);
			this.passwordTxt.Name = "passwordTxt";
			this.passwordTxt.PasswordChar = '*';
			this.passwordTxt.Size = new System.Drawing.Size(160, 20);
			this.passwordTxt.TabIndex = 10;
			this.passwordTxt.Text = "";
			//
			// repasswordTxt
			//
			this.repasswordTxt.Location = new System.Drawing.Point(112, 96);
			this.repasswordTxt.Name = "repasswordTxt";
			this.repasswordTxt.PasswordChar = '*';
			this.repasswordTxt.Size = new System.Drawing.Size(160, 20);
			this.repasswordTxt.TabIndex = 11;
			this.repasswordTxt.Text = "";
			//
			// renameTxt
			//
			this.renameTxt.Location = new System.Drawing.Point(112, 120);
			this.renameTxt.Name = "renameTxt";
			this.renameTxt.Size = new System.Drawing.Size(176, 20);
			this.renameTxt.TabIndex = 12;
			this.renameTxt.Text = "";
			//
			// addressTxt
			//
			this.addressTxt.AcceptsReturn = true;
			this.addressTxt.Location = new System.Drawing.Point(112, 144);
			this.addressTxt.Multiline = true;
			this.addressTxt.Name = "addressTxt";
			this.addressTxt.Size = new System.Drawing.Size(160, 48);
			this.addressTxt.TabIndex = 13;
			this.addressTxt.Text = "";
			//
			// emailTxt
			//
			this.emailTxt.Location = new System.Drawing.Point(112, 200);
			this.emailTxt.Name = "emailTxt";
			this.emailTxt.Size = new System.Drawing.Size(160, 20);
			this.emailTxt.TabIndex = 14;
			this.emailTxt.Text = "";
			//
			// userRegistrationForm
			//
			this.AutoScale = false;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(292, 266);
			this.ControlBox = false;
			this.Controls.Add(this.emailTxt);
			this.Controls.Add(this.addressTxt);
			this.Controls.Add(this.renameTxt);
			this.Controls.Add(this.repasswordTxt);
			this.Controls.Add(this.passwordTxt);
			this.Controls.Add(this.usernameTxt);
			this.Controls.Add(this.label7);
			this.Controls.Add(this.label6);
			this.Controls.Add(this.label5);
			this.Controls.Add(this.label4);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.cancelBtn);
			this.Controls.Add(this.registerBtn);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Name = "userRegistrationForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "User Registration";
			this.ResumeLayout(false);

		}
		#endregion

		private void cancelBtn_Click(object sender, System.EventArgs e)
		{
			this.Dispose();
		}

		private void registerBtn_Click(object sender, System.EventArgs e)
		{
			CUser user = new CUser();
			string msg=null;
			if(user.validateUserData(usernameTxt.Text, passwordTxt.Text, repasswordTxt.Text, renameTxt.Text, addressTxt.Text, emailTxt.Text, ref msg)==false)
			{
				MessageBox.Show(msg);
			}
			else
			{
				user.setUserData(usernameTxt.Text, passwordTxt.Text, renameTxt.Text, addressTxt.Text, emailTxt.Text);
				if(!user.registerUserWS())
				{
					MessageBox.Show("An error occured while registering the user on the Authentication Server!!!");

					this.Dispose();
				} else if(user.writeUserData())
				{
					MessageBox.Show("The registration was created successfully!!!");

					mf.registerUserBtn.Enabled = false;
					mf.usernameTxt.Enabled =  true;
					mf.passwordTxt.Enabled = true;
					mf.startWalletBtn.Enabled = true;

					this.Dispose();
				}
			}
		}

	}
}
