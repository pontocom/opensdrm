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
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"].'/phplibrary/include.const.php');
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"].'/phplibrary/include.debug.php');
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"].'/phplibrary/include.string.php');
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"].'/phplibrary/include.bcmath.php');
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"].'/phplibrary/include.math.php');
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"].'/phplibrary/include.prime.php');
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"].'/phplibrary/class.mersennetwister.php');
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"].'/phplibrary/class.random.php');
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"].'/phplibrary/include.rsa.php');
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"].'/phplibrary/class.rsa.php');

include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"].'/common/PublicKey.class.php');
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"].'/common/PrivateKey.class.php');

/** RSASignature class
To handle RSA signatures.
*/
class RSASignature
{
	/**
	 * Constructor function of the RSASignature.
	 * It does nothing.
	 */
	function RSASignature()
	{
	}

	/**
	 * This function verifies a signature.
	 * @param data_to_verify Data value to verify the signature
	 * @param signature Signature value to be verified
	 * @param public_key Public key object to be used on the signature verification process.
	 * @return A boolean value with true if the signature is valid, or false in the signature is invalid.
	 */
	function RSA_verify($data_to_verify, $signature, $public_key) {
		//Set_Time_Limit(2400);

		//$verifier = new CRSA();
		//$_pbkey = array("n" => MathHexToDec($public_key->pubkey["n"]),
		//				"e" => MathHexToDec($public_key->pubkey["e"]));
		//$verifier->SetPublicKey($_pbkey);
		//return $verifier->Verification($data_to_verify, MathHexToStr($signature));
		return ahrsa_verify($data_to_verify,
							pack('H'.strlen($signature), $signature),
							pack('H'.strlen($public_key->pubkey["n"]), $public_key->pubkey["n"]),
							pack('H'.strlen($public_key->pubkey["e"]), $public_key->pubkey["e"])
							);
	}

	/**
	 * This function signs some data
	 * @param data_to_sign Data value to be signed
	 * @param private_key Private key to be used on the signature process.
	 * @return A signature value in hexadecimal format.
	 */
	function RSA_sign($data_to_sign, $private_key) {
		//Set_Time_Limit(2400);

		//$signer = new CRSA();
		//$_prkey = array("n" => MathHexToDec($private_key->privkey["n"]),
		//				"e" => MathHexToDec($private_key->privkey["e"]),
		//				"d" => MathHexToDec($private_key->privkey["d"]),
		//				"p" => MathHexToDec($private_key->privkey["p"]),
		//				"q" => MathHexToDec($private_key->privkey["q"]));
		//$signer->SetPrivateKey($_prkey);
		//return MathStrToHex($signer->Signature($data_to_sign));
		return bin2hex(ahrsa_sign($data_to_sign,
									pack('H'.strlen($private_key->privkey["n"]), $private_key->privkey["n"]),
									pack('H'.strlen($private_key->privkey["e"]), $private_key->privkey["e"]),
									pack('H'.strlen($private_key->privkey["d"]), $private_key->privkey["d"]),
									pack('H'.strlen($private_key->privkey["p"]), $private_key->privkey["p"]),
									pack('H'.strlen($private_key->privkey["q"]), $private_key->privkey["q"])
									));
	}
}