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
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/phplibrary/include.const.php");
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/phplibrary/include.debug.php");
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/phplibrary/include.string.php");
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/phplibrary/include.bcmath.php");
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/phplibrary/include.math.php");
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/phplibrary/include.prime.php");
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/phplibrary/class.aes.php");


/** AESEncrypt class
To encrypt and decrypt data using a AES algorithm.
*/
class AESEncrypt
{
	/**
	 * This is the constructor function. Does nothing...
	 */
	function AESEncrypt()
	{
	}

	function AES_encrypt($data_to_encrypt, $skey) {
		//$aes = new CAES;
		//return Base64_Encode($aes->Encrypt($skey, $data_to_encrypt));
        $sk = md5($skey);
        $sk_len = strlen($sk);
        $k = pack('H'.$sk_len, $sk);
        $texto_cifrado = ahaes_cipher($data_to_encrypt, $k);
        return base64_encode($texto_cifrado);
	}

	function AES_decrypt($data_to_decrypt, $skey) {
		//$aes = new CAES;
		//return $aes->Decrypt($skey, Base64_Decode($data_to_decrypt));
        $sk = md5($skey);
        $sk_len = strlen($sk);
        $k = pack('H'.$sk_len, $sk);
        return ahaes_decipher(base64_decode($data_to_decrypt), $k);
	}
}