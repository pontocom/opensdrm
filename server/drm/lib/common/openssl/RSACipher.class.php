<?php
/*
 OpenSDRM - Open Secure Digital Rights Management
 Copyright (c) 2006 Adetti

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

/**
 * RSACipher.class,php
 *
 * This file is used to cipher and decipher information
 * using the RSA algorithm.
 * @author Carlos Serrão <carlos.j.serrao@gmail.com>
 * @version 1.0
 */

/**
* These are the required files for the class.
*/
require_once("config.inc.php");
require_once("X509Certificate.class.php");
require_once("PrivateKey.class.php");

/**
 * This is the declaration of the RSACipher class. This class will be used to cipher and decipher data using the RSA algorithm.
 */
class RSACipher
{
	/**
	* This variable holds the class public key in a resource format
	* @access public
	* @var resource
	*/
	public $_publicKey;
	/**
	* This variable holds the class private key in a resource format
	* @access private
	* @var resource
	*/
	private $_privateKey;

	/**
	* This is the default constructor of the class
	*/
	function __construct()
	{
	}

	/**
	* This is the default destructor of the class
	*/
	function __destruct()
	{
	}

	/**
	* This function encrypts some arbitrary cleartext data and returns the ciphertext without any transformation - raw encryption - padding is used
	* @param string $cleardata The data to be ciphered
	* @param X509Certificate $cert The certificate that contains the public key that will be used to cipher data
	* @return bool|string The function returns false if an error as occured or it returns a string containing the cipherdata
	*/
	public function encrypt($cleardata, $cert)
	{
		// extract the public key from the certificate
		$this->_publicKey = openssl_pkey_get_public($cert->getCert(1));

		// remove 11 bytes because of the padding
		$maxlength = ($GLOBALS['myconfig']['keysize'] - (11*8))/8;
		$output='';
		while($cleardata){
  			$input= substr($cleardata,0,$maxlength);
  			$cleardata=substr($cleardata,$maxlength);
			if(openssl_public_encrypt($input, $encrypted,$this->_publicKey)==false)
				return false;
  			$output.=$encrypted;
		}
		return $output;
	}

	/**
	* This function decrypts some arbitrary ciphertext data and returns the cleartext without any transformation - raw decryption
	* @param string $cipherdata The data to be deciphered
	* @param PrivateKey $key The certificate that contains the public key that will be used to cipher data
	* @return bool|string The function returns false if an error as occured or it returns a string containing the cleartext
	*/
	public function decrypt($cipherdata, $key)
	{
		// extract the private key from the key pair
		$this->_privateKey = openssl_pkey_get_private($key->_spKey);

		$maxlength=($GLOBALS['myconfig']['keysize'])/8;
		$output='';
		while($cipherdata){
			$input= substr($cipherdata,0,$maxlength);
			$cipherdata=substr($cipherdata,$maxlength);
			if (openssl_private_decrypt($input,$out,$this->_privateKey)==false) {
				return false;
			}
		  	$output.=$out;
		}
		return $output;
	}

	/**
	* This function verifies if a certificate matches or not a private key
	* @param X509Certificate $cert The certificate to be verified
	* @param PrivateKey $pkey The private key to be verified
	* @return bool|string The function returns false if an error as occured
	*/
	public function testCert($cert, $pkey){
		return openssl_x509_check_private_key($cert->getCert(), $pkey->_pKey);
	}
}

/*
// Class tests
$x509CA = new X509Certificate();
$x509CA->loadCert("UserCert.cer");
//$x509CA->loadCert("AUSCert.cer");
$pCA = new PrivateKey();
$pCA->load("UserPKey.pem");
//$pCA->load("AUSKeys.pem");

$testStr= <<<EOT
TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345
TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345
TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345
TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345
TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345
TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345
TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345
TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345
TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345
TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345
TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345
TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345
TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345
TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345
TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345
TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345
TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345
TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345
EOT;

$testStr2= <<<EOT
This is a simple class to use OpenSSL
EOT;

$rsa = new RSACipher();
$cipherdata = $rsa->encrypt($testStr, $x509CA);

var_dump($x509CA->parseCert());

print("Cipherdata = ".$cipherdata);

$data = $rsa->decrypt($cipherdata, $pCA);
print("Data = ".$data);

print("Match = ".$rsa->testCert($x509CA, $pCA));
*/
?>