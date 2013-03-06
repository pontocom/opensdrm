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
using System.Xml;
using Microsoft.Win32;

namespace osdrmWallet
{
	/// <summary>
	/// Summary description for WalletClientHdlr.
	/// </summary>
	public class WalletClientHdlr
	{
		private TcpClient ligCli;
		Thread thrCli;
		private string uname, pwd;

		public WalletClientHdlr(TcpClient lig, string uname, string pwd)
		{
			//
			// TODO: Add constructor logic here
			//
			ligCli = lig;

			this.uname = uname;
			this.pwd = pwd;

			thrCli = new Thread(new ThreadStart(this.handleRequest));
			thrCli.IsBackground = true;
			thrCli.Start();
		}

		private void handleRequest()
		{
			StreamWriter wCli = null;
			StreamReader rCli = null;

			try
			{
				NetworkStream strLig = ligCli.GetStream();
				wCli = new StreamWriter(strLig);
				rCli = new StreamReader(strLig);

				wCli.AutoFlush = true;

				string data=null;
				data = rCli.ReadLine();

				//System.Windows.Forms.MessageBox.Show(data);

				//char[] _data = data.ToCharArray();
				// Here we should receive a GETKEY urn:mpeg:mpeg21:diid:doi:pt/adetti/music/00000005
				// Here we shoudl receive a GETKEY MD5HASH

				// we should return the key, if the license hasn't expired yet (playcount)
				//System.Windows.Forms.MessageBox.Show(data.Substring(0,6));
				if(String.Compare(data.Substring(0,6),"GETKEY")==0 || String.Compare(data.Substring(0,6),"getkey")==0) // This is a valid request
				{
					char[] _data = data.ToCharArray(); // convert to chars to extract the image ID and level

					int n=0;
					string CID = null;
					for(n=7; n<data.Length; n++)
					{
						if(_data[n]!=' ')
							CID += _data[n];
					}

					//System.Windows.Forms.MessageBox.Show(":"+CID+":");

					// now we have the content id

					if (CID==null)
					{
						wCli.Write("ERROR");
						System.Windows.Forms.MessageBox.Show("The content identifier is empty!!!");
					}
					else
					{

						// the wallet needs to do the following:
						// 1. Check wheter a license for that image is already on the system

						if(checkLicenseOnSystem(CID))
						{
							// ok, we already have the license on our system
							// we need to read the license from the file
							string license = null;
							CUser cu = new CUser();
							if(!cu.readUserData(this.uname, this.pwd))
							{
								wCli.Write("ERROR");
								System.Windows.Forms.MessageBox.Show("An error has occured while trying to read the data file.");
							}
							else
							{
								if(readLicenseFromSystem(CID, this.uname, this.pwd, ref license))
								{
									string key = null;
									if(checkLicenseValidityPlaycount(CID))
									{
										if(getRequestedKey(license, ref key))
										{
											wCli.WriteLine(key);
										}
										else
										{
											wCli.Write("ERROR");
										}
									}
									else
									{
										// delete current license file
										deleteLicenseFile(CID);
										// try to get a new one from the license server ????
										wCli.Write("ERROR");
									}
								}
								else
								{
									wCli.Write("ERROR");
								}
							}
						}
						else // we don't have the license file, we need to obtain it remotely from License Server
						{
							string license = null;
							CUser cu = new CUser();
							if(!cu.readUserData(this.uname, this.pwd))
							{
								wCli.Write("ERROR");
								System.Windows.Forms.MessageBox.Show("An error has occured while trying to read the data file.");
							}
							else
							{
								//System.Windows.Forms.MessageBox.Show(cu.getUid());
								if(obtainLicenseFromWS(cu.getUid(), CID, this.uname, this.pwd, ref license))
								{
									//System.Windows.Forms.MessageBox.Show(license);
									if(createLicenseFile(CID, this.uname, this.pwd, license))
									{
										string key = null;
										if(checkLicenseValidityPlaycount(CID))
										{
											if(getRequestedKey(license, ref key))
											{
												wCli.WriteLine(key);
											}
											else
											{
												wCli.Write("ERROR");
											}
										}
										else
										{
											wCli.Write("ERROR");
										}
									}
									else
									{
										wCli.Write("ERROR");
									}
								}
								else
								{
									wCli.Write("ERROR");
								}
							}
						}
					}

					//wCli.WriteLine("RES_LVL="+RES_LVL);
					//wCli.WriteLine("id_imagem="+IMG_ID);
				}
				else // return ERROR
				{
					wCli.Write("ERROR");
				}

				//System.Windows.Forms.MessageBox.Show(data);
			}
			catch (Exception e)
			{
				wCli.Write("ERROR");
				System.Windows.Forms.MessageBox.Show(e.Message);
				System.Windows.Forms.MessageBox.Show(e.StackTrace);
			}
			finally
			{
				if(wCli!=null)
					wCli.Close();
				if(rCli!=null)
					rCli.Close();
				if(ligCli!=null)
					ligCli.Close();
			}
		}

		public bool checkLicenseOnSystem(string cid)
		{
			/** check on filesystem if the license file exists or not */

			string fname=cid.Replace("/","");
			string filename=fname.Replace(":","");
			FileInfo licfile = new FileInfo(@".\data\"+filename);
			if(licfile.Exists)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		public bool obtainLicenseFromWS(string uid, string cid, string username, string password, ref string rlicense)
		{
			//string authdata = "<auth><username>"+username+"</username><password>"+password+"</password></auth>";

			//XmlDocument doc = new XmlDocument();

			//System.Windows.Forms.MessageBox.Show(uid);
			//System.Windows.Forms.MessageBox.Show(cid);


			// call the web service
			string license=null;
			osdrmLIS.opensdrmws ws = new osdrmWallet.osdrmLIS.opensdrmws();
			//osdrmh2kLIS1.opensdrmws ws = new osdrmWallet.osdrmh2kLIS1.opensdrmws();

			string retv="";

			if(cid.Length==32)
			{ // it is the SPECIAL CASE of an hash
				retv=ws.LISrequestLicenseDownloadSpecial(uid, cid, "", out license);
			}
			else
			{
				retv=ws.LISrequestLicenseDownload(uid, cid, "", out license);
			}

			//System.Windows.Forms.MessageBox.Show(retv);
			//System.Windows.Forms.MessageBox.Show(license);

			if(retv.ToCharArray()[0]=='-')
			{
				return false;
			}
			else if(retv.ToCharArray()[0]=='+')
			{
				rlicense = license;
				return true;
			}
			return false;
		}

		public bool readLicenseFromSystem(string cid, string username, string password, ref string rlicense)
		{
			string fname=cid.Replace("/","");
			string filename=fname.Replace(":","");
			FileStream fs = new FileStream(@".\data\"+filename, FileMode.Open);
			StreamReader sr = new StreamReader(fs);

			CUser cu = new CUser();
			rlicense = cu.decipherDataS(username,password,sr.ReadLine());

			sr.Close();

			return true;
		}

		public bool createLicenseFile(string cid, string uname, string pwd, string license)
		{
			string fname=cid.Replace("/","");
			string filename=fname.Replace(":","");
			FileStream fs = new FileStream(@".\data\"+filename, FileMode.Create);
			StreamWriter sw = new StreamWriter(fs);

			CUser cu = new CUser();
			sw.WriteLine(cu.cipherDataS(uname, pwd, license));

			sw.Close();


			/** create the appropriate values in the registry */
			RegistryKey rk = Registry.CurrentUser.OpenSubKey(@"Software\OpenSDRMWallet", true);
			rk.CreateSubKey(filename);
			rk.Close();

			rk = Registry.CurrentUser.OpenSubKey(@"Software\OpenSDRMWallet\"+filename, true);
			rk.SetValue("playcount", "0");
			rk.Close();

			/** now we need to parse the license and get the playcount */
			string pc = getPCFromLicense(license);

			/** insert the original playcount on the Registry */
			rk = Registry.CurrentUser.OpenSubKey(@"Software\OpenSDRMWallet\"+filename, true);
			rk.SetValue("playcount", pc);
			rk.Close();


			return true;
		}

		public void deleteLicenseFile(string cid)
		{
			string fname=cid.Replace("/","");
			string filename=fname.Replace(":","");

			FileInfo fi = new FileInfo(@".\data\"+filename);

			if(fi.Exists)
				fi.Delete();


			/** delete also the key from the registry */
			RegistryKey rk = Registry.CurrentUser.OpenSubKey(@"Software\OpenSDRMWallet", true);
			rk.DeleteSubKeyTree(filename);
			rk.Close();

		}

		public bool getRequestedKey(string license, ref string key)
		{
			XmlDocument doc = new XmlDocument();

			doc.LoadXml(license);

			XmlElement root = doc.DocumentElement;
			XmlNodeList elemList = root.GetElementsByTagName("ds:keyValue");

			if(elemList.Count != 0)
			{
				key = elemList[0].InnerXml;
				return true;
			}
			else
			{
				return false;
			}

			/*string skey = null;
			for(int n=0; n < elemList.Count; n++)
			{
				if(checkKeyLevel(elemList[n].OuterXml, res_lvl, ref skey))
				{
					key = skey;
					return true;
				}
			}*/
			//System.Windows.Forms.MessageBox.Show(elemList[n].OuterXml);

		}

		public String getPCFromLicense(string license)
		{
			XmlDocument doc = new XmlDocument();

			doc.LoadXml(license);

			XmlElement root = doc.DocumentElement;
			XmlNodeList elemList = root.GetElementsByTagName("o-dd:count");

			//System.Windows.Forms.MessageBox.Show((elemList.Count).ToString());

			string pc = elemList[0].InnerXml;

			return pc;
		}

		public bool checkKeyLevel(string xml, string level, ref string key)
		{
			XmlDocument doc = new XmlDocument();

			doc.LoadXml(xml);

			XmlElement root = doc.DocumentElement;
			//XmlNodeList elemList=root.GetElementsByTagName("key");
			string tmpkey = root.InnerXml;

			string lvl = root.GetAttribute("level");

			if(lvl.CompareTo(level)==0)
			{
				key = tmpkey;
				return true;
			}
			else
			{
				return false;
			}
		}

		public bool checkLicenseValidityDate(string license)
		{
			XmlDocument doc = new XmlDocument();

			doc.LoadXml(license);

			XmlElement root = doc.DocumentElement;
			XmlNodeList bList=root.GetElementsByTagName("notbefore");
			XmlNodeList aList=root.GetElementsByTagName("notafter");

			string sdate = bList[0].InnerXml;
			string edate = aList[0].InnerXml;

			//string now = DateTime.Now.Day.ToString()+"-"+DateTime.Now.Month.ToString()+"-"+DateTime.Now.Year.ToString();

			string sd, sm, sy, ed, em, ey;
			char[] _sdate = sdate.ToCharArray();
			char[] _edate = edate.ToCharArray();

			sd = ""+_sdate[0]+_sdate[1];
			sm = ""+_sdate[3]+_sdate[4];
			sy = ""+_sdate[6]+_sdate[7]+_sdate[8]+_sdate[9];

			ed = ""+_edate[0]+_edate[1];
			em = ""+_edate[3]+_edate[4];
			ey = ""+_edate[6]+_edate[7]+_edate[8]+_edate[9];

			DateTime startDate = new DateTime(Convert.ToInt16(sy), Convert.ToInt16(sm), Convert.ToInt16(sd), 0, 0, 0, 0);
			DateTime endDate = new DateTime(Convert.ToInt16(ey), Convert.ToInt16(em), Convert.ToInt16(ed), 23, 59, 59, 0);

			DateTime now = DateTime.Now;

			if(now.CompareTo(startDate) >= 0)
			{
				if (now.CompareTo(endDate) <= 0)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}

		public bool checkLicenseValidityPlaycount(string cid)
		{
			/*XmlDocument doc = new XmlDocument();

			doc.LoadXml(license);

			XmlElement root = doc.DocumentElement;
			XmlNodeList bList=root.GetElementsByTagName("o-dd:count");

			string count = bList[0].InnerXml;

			System.Windows.Forms.MessageBox.Show("count = "+count);*/

			/*if(license > 0)
			{
				license --;
				// escreve novo valor se maior que 0
			}
			else
			{
			}*/

			string fname=cid.Replace("/","");
			string filename=fname.Replace(":","");


			RegistryKey rk = Registry.CurrentUser.OpenSubKey(@"Software\OpenSDRMWallet\"+filename, true);
			if(rk == null)
			{
				rk.Close();
				return false;
			}
			else
			{
				int actualpc = int.Parse(rk.GetValue("playcount").ToString());

				if(actualpc > 0)
				{
					int newpc = actualpc-1;

					rk.SetValue("playcount", newpc.ToString());
					rk.Close();

					return true;
				}
				else
				{
					rk.Close();
					return false;
				}
			}

		}

		public void killHandler()
		{
			thrCli.Abort();
		}
	}
}
