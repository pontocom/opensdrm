<?
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

//include_once(getenv('OPENSDRM_INSTALL_DIR')."config.inc.php");
include_once($_SERVER['DOCUMENT_ROOT']."/config.inc.php");
//require_once($_SERVER['DOCUMENT_ROOT'].$GLOBALS["LIB_DIR"].'/phplibrary/include.const.php');
//require_once($_SERVER['DOCUMENT_ROOT'].$GLOBALS["LIB_DIR"].'/phplibrary/include.debug.php');
//require_once($_SERVER['DOCUMENT_ROOT'].$GLOBALS["LIB_DIR"].'/phplibrary/include.string.php');
//require_once($_SERVER['DOCUMENT_ROOT'].$GLOBALS["LIB_DIR"].'/phplibrary/include.bcmath.php');
//require_once($_SERVER['DOCUMENT_ROOT'].$GLOBALS["LIB_DIR"].'/phplibrary/include.math.php');
//require_once($_SERVER['DOCUMENT_ROOT'].$GLOBALS["LIB_DIR"].'/phplibrary/include.prime.php');
//require_once($_SERVER['DOCUMENT_ROOT'].$GLOBALS["LIB_DIR"].'/phplibrary/class.mersennetwister.php');
//require_once($_SERVER['DOCUMENT_ROOT'].$GLOBALS["LIB_DIR"].'/phplibrary/class.random.php');
//require_once($_SERVER['DOCUMENT_ROOT'].$GLOBALS["LIB_DIR"].'/phplibrary/include.rsa.php');
//require_once($_SERVER['DOCUMENT_ROOT'].$GLOBALS["LIB_DIR"].'/phplibrary/class.rsa.php');

include_once($_SERVER['DOCUMENT_ROOT'].$GLOBALS["LIB_DIR"].'/common/PublicKey.class.php');
include_once($_SERVER['DOCUMENT_ROOT'].$GLOBALS["LIB_DIR"].'/common/PrivateKey.class.php');

/** RSAEncrypt class
To encrypt and decrypt data using a RSA algorithm.
*/
class RSAEncrypt
{
	/**
	 * This is the constructor function. Does nothing...
	 */
	function RSAEncrypt()
	{
	}

	/**
	 * This function encrypts a value using a RSA public key.
	 * @param data_to_encrypt Data to be encrypted using RSA
	 * @param public_key Public key object, containing the public key value.
	 * @return The encrypted data Base64 encoded.
	 */
	function RSA_encrypt($data_to_encrypt, $public_key) {
		//Set_Time_Limit(2400);

		//$encriptor = new CRSA();
		//$_pbkey = array("n" => MathHexToDec($public_key->pubkey["n"]),
		//				"e" => MathHexToDec($public_key->pubkey["e"]));
		//$encriptor->SetPublicKey($_pbkey);
		//return Base64_Encode($encriptor->Encrypt($data_to_encrypt));
		return bin2hex(ahrsa_cipher($data_to_encrypt,
						pack('H'.strlen($public_key->pubkey["n"]), $public_key->pubkey["n"]),
						pack('H'.strlen($public_key->pubkey["e"]), $public_key->pubkey["e"])
						));

	}

	/**
	 * This function decrypts data using an RSA private key
	 * @param data_to_decrypt Datat to be decrypted in Base64 format, using RSA.
	 * @param private_key Private key object, containing the private key value.
	 * @return The plain text data.
	 */
	function RSA_decrypt($data_to_decrypt, $private_key) {
		//Set_Time_Limit(2400);

		//$decriptor = new CRSA();
		//$_prkey = array("n" => MathHexToDec($private_key->privkey["n"]),
		//				"e" => MathHexToDec($private_key->privkey["e"]),
		//				"d" => MathHexToDec($private_key->privkey["d"]),
		//				"p" => MathHexToDec($private_key->privkey["p"]),
		//				"q" => MathHexToDec($private_key->privkey["q"]));
		//$decriptor->SetPrivateKey($_prkey);
		//return $decriptor->Decrypt(Base64_Decode($data_to_decrypt));
		return bin2hex(ahrsa_cipher($data_to_decrypt,
									pack('H'.strlen($private_key->privkey["n"]), $private_key->privkey["n"]),
									pack('H'.strlen($private_key->privkey["e"]), $private_key->privkey["e"]),
									pack('H'.strlen($private_key->privkey["d"]), $private_key->privkey["d"]),
									pack('H'.strlen($private_key->privkey["p"]), $private_key->privkey["p"]),
									pack('H'.strlen($private_key->privkey["q"]), $private_key->privkey["q"])
									));
	}
}