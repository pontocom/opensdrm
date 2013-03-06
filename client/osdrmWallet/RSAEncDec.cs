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
using System.Security.Cryptography;
using System.Text;

class RSAEncDec
{
	public RSAParameters rsaki;
	public int rsaks;

	public RSAEncDec()
	{
	}

/*	static void Main()
	{
		try
		{
			//Create a UnicodeEncoder to convert between byte array and string.
			UnicodeEncoding ByteConverter = new UnicodeEncoding();

			//Create byte arrays to hold original, encrypted, and decrypted data.
			byte[] dataToEncrypt = ByteConverter.GetBytes("Data to Encrypt");
			byte[] encryptedData;
			byte[] decryptedData;

			//Create a new instance of RSACryptoServiceProvider to generate
			//public and private key data.
			RSACryptoServiceProvider RSA = new RSACryptoServiceProvider();

			//Pass the data to ENCRYPT, the public key information
			//(using RSACryptoServiceProvider.ExportParameters(false),
			//and a boolean flag specifying no OAEP padding.
			encryptedData = RSAEncrypt(dataToEncrypt,RSA.ExportParameters(false), false);

			//Pass the data to DECRYPT, the private key information
			//(using RSACryptoServiceProvider.ExportParameters(true),
			//and a boolean flag specifying no OAEP padding.
			decryptedData = RSADecrypt(encryptedData,RSA.ExportParameters(true), false);

			//Display the decrypted plaintext to the console.
			Console.WriteLine("Decrypted plaintext: {0}", ByteConverter.GetString(decryptedData));
		}
		catch(ArgumentNullException)
		{
			//Catch this exception in case the encryption did
			//not succeed.
			Console.WriteLine("Encryption failed.");

		}
	}
*/

	public void RSAcreatekeys(int size)
	{
		//Generate a public/private key pair.
		RSACryptoServiceProvider RSA = new RSACryptoServiceProvider(size);
		rsaks = size;

		//Save the public key information to an RSAParameters structure.
		rsaki = RSA.ExportParameters(true);
	}

	public string returnPublicComponents()
	{
		//return "<publicKey><algorithmIdentifier>RSA-"+rsaks+"</algorithmIdentifier><component>"+Convert.ToBase64String(rsaki.Modulus)+"</component><component>"+Convert.ToBase64String(rsaki.Exponent)+"</component></publicKey>";
		return "<publicKey><algorithmIdentifier>RSA-"+rsaks+"</algorithmIdentifier><component>"+byte2Hex(rsaki.Modulus)+"</component><component>"+byte2Hex(rsaki.Exponent)+"</component></publicKey>";
	}

	private string byte2Hex(byte[] tocv)
	{
		string cvtd = null;
		for(int n=0; n<tocv.Length; n++)
		{
			cvtd += tocv[n].ToString("X2");
		}
		return cvtd;
	}

	public string returnPrivateComponents()
	{
		//return "<privateKey><algorithmIdentifier>RSA-"+rsaks+"</algorithmIdentifier><component>"+Convert.ToBase64String(rsaki.Modulus)+"</component><component>"+Convert.ToBase64String(rsaki.Exponent)+"</component><component>"+Convert.ToBase64String(rsaki.D)+"</component><component>"+Convert.ToBase64String(rsaki.P)+"</component><component>"+Convert.ToBase64String(rsaki.Q)+"</component></privateKey>";
		return "<privateKey><algorithmIdentifier>RSA-"+rsaks+"</algorithmIdentifier><component>"+byte2Hex(rsaki.Modulus)+"</component><component>"+byte2Hex(rsaki.Exponent)+"</component><component>"+byte2Hex(rsaki.D)+"</component><component>"+byte2Hex(rsaki.P)+"</component><component>"+byte2Hex(rsaki.Q)+"</component></privateKey>";
	}

	public byte[] RSAEncrypt(byte[] DataToEncrypt, RSAParameters RSAKeyInfo, bool DoOAEPPadding)
	{
		try
		{
			//Create a new instance of RSACryptoServiceProvider.
			RSACryptoServiceProvider RSA = new RSACryptoServiceProvider();

			//Import the RSA Key information. This only needs
			//toinclude the public key information.
			RSA.ImportParameters(RSAKeyInfo);

			//Encrypt the passed byte array and specify OAEP padding.
			//OAEP padding is only available on Microsoft Windows XP or
			//later.
			return RSA.Encrypt(DataToEncrypt, DoOAEPPadding);
		}
			//Catch and display a CryptographicException
			//to the console.
		catch(CryptographicException e)
		{
			Console.WriteLine(e.Message);

			return null;
		}

	}

	public byte[] RSADecrypt(byte[] DataToDecrypt, RSAParameters RSAKeyInfo,bool DoOAEPPadding)
	{
		try
		{
			//Create a new instance of RSACryptoServiceProvider.
			RSACryptoServiceProvider RSA = new RSACryptoServiceProvider();

			//Import the RSA Key information. This needs
			//to include the private key information.
			RSA.ImportParameters(RSAKeyInfo);

			//Decrypt the passed byte array and specify OAEP padding.
			//OAEP padding is only available on Microsoft Windows XP or
			//later.
			return RSA.Decrypt(DataToDecrypt, DoOAEPPadding);
		}
			//Catch and display a CryptographicException
			//to the console.
		catch(CryptographicException e)
		{
			Console.WriteLine(e.ToString());

			return null;
		}

	}
}
