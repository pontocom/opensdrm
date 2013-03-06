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
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/phplibrary/class.mersennetwister.php");
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/phplibrary/class.random.php");
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/phplibrary/include.rsa.php");
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/phplibrary/class.rsa.php");


include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/PublicKey.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/PrivateKey.class.php");


/** KeyGenerator class
To generate the RSA key pair
NOTE: Creating RSA key pairs in PHP is quite slow. This library would need to be replaced by another one (Java ?) in a future release.
*/

class KeyGenerator
{
	var $pubkey; /**< an object containing the public key */
	var $privkey; /**< an object containing the private key */

	/**
	 * Constructor function of the KeyGenerator.
	 * Declares the two keys objects.
	 */
	function KeyGenerator()
	{
		$this->pubkey = new PublicKey();
		$this->privkey = new PrivateKey();
	}

	/**
	 * This function generates an RSA key pair (This is soooo slow). The keys are then stored in the appropriate array in hexadecimal values.
	 * @param size The size of the key pair being generated in bits.
	 */
	function RSAKeyGenerate($size) {
		//Set_Time_Limit(2400);

		//$crsa = new CRSA;
		//$crsa->CreatePrimes("9876543210", $size);
		//$crsa->CreatePrimes(time(), $size);
		//$crsa->CalcKeys();

		$crsa = ahrsa_create_keys($size);

		$this->pubkey->set("RSA", bin2hex($crsa['n']), bin2hex($crsa['e']));
		$this->privkey->set("RSA", bin2hex($crsa['n']), bin2hex($crsa['e']), bin2hex($crsa['d']), bin2hex($crsa['p']), bin2hex($crsa['q']));
	}

	/**
	 * This function displays the RSA key pair.
	 */
	function displayKeys() {
		print_r($this->pubkey->pubkey);
		print_r($this->privkey->privkey);
	}
}