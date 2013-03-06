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
using System.Net;
using System.IO;
using System.Security.Cryptography;
using System.Text;
using System.Management;
using System.Xml;
using Microsoft.Win32;


namespace osdrmWallet
{
	/// <summary>
	/// Summary description for CUser.
	/// </summary>
	public class CUser
	{
		private string user_identifier;
		private string username;
		private string password;
		private string realname;
		private string address;
		private string email;

		private string pkey, privkey;

		public CUser()
		{
			//
			// TODO: Add constructor logic here
			//
		}

		public void setUserData(string username, string password, string realname, string address, string email)
		{
			this.username = username;
			this.password = password;
			this.realname = realname;
			this.address = address;
			this.email = email;
		}

		public bool validateUserData(string usernameTxt, string passwordTxt, string repasswordTxt, string renameTxt, string addressTxt, string emailTxt, ref string message)
		{
			if(usernameTxt=="")
			{
				message = "Username field cannot be empty!";
				return false;
			}
			else if(passwordTxt=="")
			{
				message = "Password field cannot be empty!";
				return false;
			}
			else if(repasswordTxt=="")
			{
				message = "Second password field cannot be empty!";
				return false;
			}
			else if(renameTxt=="")
			{
				message = "Real name password cannot be empty!";
				return false;
			}
			else if(addressTxt=="")
			{
				message = "Address field cannot be empty!";
				return false;
			}
			else if(emailTxt=="")
			{
				message = "Email address cannot be empty!";
				return false;
			}
			else if (passwordTxt != repasswordTxt)
			{
				message = "The two passords cannot be different!!!";
				return false;
			}
			return true;
		}

		public bool registerUserHTTP()
		{
			// Register the user using the HTTP protocol !!!
			// Still have to handle with WSDL and SOAP problems in PHP/.NET connections

			try
			{
				WebClient wclient = new WebClient();
				Stream strclient = wclient.OpenRead("http://localhost/opensdrmWS/AUS/AUS.nws.php?op=AUS3");
				StreamReader pag = new StreamReader(strclient);

				string linha=null;

				do
				{
					linha = pag.ReadLine();
					System.Windows.Forms.MessageBox.Show(linha);
				} while (linha!=null);

				strclient.Close();

				//System.Windows.Forms.MessageBox.Show(linha);
			}
			catch (Exception e)
			{
				System.Windows.Forms.MessageBox.Show(e.ToString());
			}

			return true;
		}

		public bool registerUserWS()
		{
			/* create the RSA parameters */
			RSAEncDec rsa = new RSAEncDec();
			rsa.RSAcreatekeys(1024);

			pkey = rsa.returnPublicComponents();
			privkey = rsa.returnPrivateComponents();

			osdrmAUS.opensdrmws ws = new osdrmWallet.osdrmAUS.opensdrmws();

			//string list = null;
			//string retv = ws.AUSrequestListOfPGW("xpto", "rsa_with_md5", "LOGIN_PASSWORD", "", out list);
			string user_id = null;
			//string retv = ws.AUSrequestUserSubscription("id", "RSA_WITH_MD5", realname, address, email, "LOGINPASSWORD", "pkey", username, password, "WALLET", "", out user_id);
			//System.Windows.Forms.MessageBox.Show(pkey);
			string retv = ws.AUSrequestUserSubscription("id", "RSA_WITH_MD5", realname, address, email, "PUBLICKEY", username, password, pkey, "WALLET", "", out user_id);
			//System.Windows.Forms.MessageBox.Show(retv);
			//System.Windows.Forms.MessageBox.Show(user_id);

			if(retv.ToCharArray()[0]=='-')
			{
				return false;
			}
			else
			{
				user_identifier = user_id;
				return true;
			}
		}

		public bool writeUserData()
		{
			try
			{
				DirectoryInfo di = new DirectoryInfo(@".\data");
				if(!di.Exists)
					di.Create();

				FileStream fs = new FileStream(@".\data\data.info", FileMode.Create);
				StreamWriter sw = new StreamWriter(fs);

				/* cipher the data according to the parameters specified by the user */
				sw.WriteLine(cipherDataS(username, password, user_identifier));
				//sw.WriteLine(user_identifier);
				sw.WriteLine(cipherDataS(username, password, username));
				sw.WriteLine(cipherDataS(username, password, password));
				sw.WriteLine(cipherDataS(username, password, realname));
				sw.WriteLine(cipherDataS(username, password, address));
				sw.WriteLine(cipherDataS(username, password, email));
				sw.WriteLine(cipherDataS(username, password, pkey));
				//sw.WriteLine(pkey);
				sw.WriteLine(cipherDataS(username, password, privkey));

				sw.Close();

				return true;
			}
			catch(Exception e)
			{
				System.Windows.Forms.MessageBox.Show(e.ToString());
				return false;
			}
		}

		public bool readUserData(string uname, string pwd)
		{
			try
			{
				DirectoryInfo di = new DirectoryInfo(@".\data");
				if(!di.Exists)
					return false;

				FileStream fs = new FileStream(@".\data\data.info", FileMode.Open);
				StreamReader sr = new StreamReader(fs);
				//StreamWriter sw = new StreamWriter(fs);

				/* cipher the data according to the parameters specified by the user */
				this.user_identifier=decipherDataS(uname, pwd, sr.ReadLine());
				this.username=decipherDataS(uname, pwd, sr.ReadLine());
				this.password=decipherDataS(uname, pwd, sr.ReadLine());
				this.realname=decipherDataS(uname, pwd, sr.ReadLine());
				this.address=decipherDataS(uname, pwd, sr.ReadLine());
				this.email=decipherDataS(uname, pwd, sr.ReadLine());
				this.pkey=decipherDataS(uname, pwd, sr.ReadLine());
				this.privkey=decipherDataS(uname, pwd, sr.ReadLine());

				sr.Close();

				return true;
			}
			catch(Exception e)
			{
				System.Windows.Forms.MessageBox.Show(e.ToString());
				return false;
			}
		}

		public bool checkDataFiles()
		{
			try
			{
				FileInfo datafile = new FileInfo(@".\data\data.info");
				if(datafile.Exists)
				{
					return true; // the file doesn't exist
				}
				else
				{
					return false; // the file exists
				}
			}
			catch (Exception e)
			{
				System.Windows.Forms.MessageBox.Show(e.ToString());
				return false;
			}
		}

		public bool validateLogin(string username, string password)
		{
			/* we try to compare the data entered by the user, with the data stored on the file */
			if(username == "" || password == "")
			{
				return false;
			}
			try
			{
				FileInfo datafile = new FileInfo(@".\data\data.info");
				if(datafile.Exists)
				{
					FileStream fs = new FileStream(@".\data\data.info", FileMode.Open);
					StreamReader sr = new StreamReader(fs);

					string _user_id = sr.ReadLine();
					string _username = sr.ReadLine();
					string _password = sr.ReadLine();
					string _realname = sr.ReadLine();
					string _address = sr.ReadLine();
					string _email = sr.ReadLine();
					string _kpub = sr.ReadLine();
					string _kpriv = sr.ReadLine();

					if(decipherDataS(username, password, _username) == username && decipherDataS(username, password, _password) == password)
					{
						user_identifier = decipherDataS(username, password, _user_id);
						username = decipherDataS(username, password, _username);
						password = decipherDataS(username, password, _password);
						realname = decipherDataS(username, password, _realname);
						address = decipherDataS(username, password, _address);
						email = decipherDataS(username, password, _email);
						pkey = decipherDataS(username, password, _kpub);
						privkey = decipherDataS(username, password, _kpriv);

						sr.Close();
						return true; // valid login operation
					}
					else
					{
						sr.Close();
						return false; // invalid login operation
					}
				}
				else
				{
					System.Windows.Forms.MessageBox.Show("Couldn't find the data file!!!");
					return false; // the file doesn't exist
				}
			}
			catch (Exception e)
			{
				System.Windows.Forms.MessageBox.Show(e.ToString());
				return false;
			}
		}

		public string cipherDataS(string login, string password, string dataToCipher)
		{
			string key = computeAESSkey(login, password);

			EncDec aes = new EncDec();
			string ciphered = aes.Encrypt(dataToCipher, key);

			return ciphered;
		}


		public string decipherDataS(string login, string password, string dataToDecipher)
		{
			string key = computeAESSkey(login, password);

			EncDec aes = new EncDec();

			string deciphered = aes.Decrypt(dataToDecipher, key);

			return deciphered;
		}

		public string getUid()
		{
			XmlDocument doc = new XmlDocument();
			doc.LoadXml(this.user_identifier);

			XmlElement root = doc.DocumentElement;
			XmlNodeList elemList = root.GetElementsByTagName("identification");

			return elemList[1].InnerXml;
		}

		private string computeAESSkey(string login, string password)
		{

			/**
			 *
				HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ProductName"
				HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ProductId"
				HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "CurrentVersion"
				HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "CurrentBuildNumber"
				HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "RegisteredOrganization"
				HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "RegisteredOwner"
				HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\OemInfo", "WbemOem"
				HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\OemInfo", "WbemProduct"
				HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", "~MHz"
				HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", "Identifier"
				HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", "VendorIdentifier"
				HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System", "SystemBiosVersion"
				HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System", "SystemBiosDate"
				HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer", "Logon User Name"
				HKEY_CURRENT_USER, "Control Panel\\International", "sCountry"
				HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ComputerName", "ComputerName"
				HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ActiveComputerName", "ActiveComputerName"
				HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters", "Hostname"
			 */
			// concatenate the key with the login and password, plus the MAC address of the computer

			// get the MAC address
			//string mac_address = null;
			//ManagementObjectSearcher query = new ManagementObjectSearcher("Select MacAddress,IPAddress from Win32_NetworkAdapterConfiguration where IPEnabled=TRUE") ;

			//ManagementObjectCollection queryCollection = query.Get();

			//foreach( ManagementObject mo in queryCollection )
			//{
			//	mac_address += mo["MacAddress"].ToString();
				//MessageBox.Show(mo["MacAddress"].ToString());
				// Print MAC address
			//}

			RegistryKey rk = Registry.LocalMachine.OpenSubKey("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion");
			string pname = rk.GetValue("ProductName").ToString();
			string pid = rk.GetValue("ProductId").ToString();
			string cv = rk.GetValue("CurrentVersion").ToString();
			string cbn = rk.GetValue("CurrentBuildNumber").ToString();
			string ro = rk.GetValue("RegisteredOrganization").ToString();
			string row = rk.GetValue("RegisteredOwner").ToString();

			RegistryKey rk2 = Registry.LocalMachine.OpenSubKey("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0");
			string cpid = rk2.GetValue("Identifier").ToString();
			string cpvi = rk2.GetValue("VendorIdentifier").ToString();

			RegistryKey rk3 = Registry.LocalMachine.OpenSubKey("HARDWARE\\DESCRIPTION\\System");
			string sbv = rk3.GetValue("SystemBiosVersion").ToString();
			string sbd = rk3.GetValue("SystemBiosDate").ToString();

			RegistryKey rk4 = Registry.LocalMachine.OpenSubKey("SYSTEM\\MountedDevices");
			string dos = rk4.GetValue(@"\DosDevices\C:").ToString();

			string key_seed = login+password+pname+pid+cv+cbn+ro+row+cpid+cpvi+sbv+sbd+dos;
			//System.Windows.Forms.MessageBox.Show(key_seed);

			ASCIIEncoding myAscii = new ASCIIEncoding();

			// the real key is the result of hashing everything
			MD5 md5 = new MD5CryptoServiceProvider();
			byte[] key = md5.ComputeHash(myAscii.GetBytes(key_seed));

			//System.Windows.Forms.MessageBox.Show(myAscii.GetString(key));
			//System.Windows.Forms.MessageBox.Show(key.Length.ToString());

			return myAscii.GetString(key);
		}
	}
}
