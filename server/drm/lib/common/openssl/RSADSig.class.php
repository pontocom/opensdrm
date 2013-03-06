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
 * RSADSig.class,php
 *
 * This file is used to sign and verify information related to digital signatures
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
 * This is the declaration of the RSADSig class. This class will be used to digitally sign blocks of information and to verify digital signatures
 */
class RSADSig
{
	/**
	* This is the default constructor of the class
	*/
	function __construct(){

	}

	/**
	* This is the default destructor of the class
	*/
	function __destruct(){

	}

	/**
	 * This function is used to sign an arbitrary block of data using RSA (private key) and using the SHA-1 hasing algorithm
	 *
	 * @param string $data The data to be signed
	 * @param PrivateKey $privateKey The object tha holds the signer private key
	 * @return string The raw digital signature of the data block
	 */
	public function sign($data, $privateKey){
		$pKey = openssl_pkey_get_private($privateKey->_pKey, null);
		$signature = null;
		openssl_sign($data, $signature, $pKey, OPENSSL_ALGO_SHA1);

		return $signature;
	}

	/**
	 * This function is used to perform signature validations from the a previously signed data block. The function will compare the supplied signature with the generated one to ensure that the data hasn't been tampered.
	 *
	 * @param string $data The data that was previously signed.
	 * @param string $signature The raw signature data.
	 * @param X509Certificate $certificate The digital certificate that contains the public-key that can be used to validate the signature
	 * @return int A value indicating if the signature is valid or not (1 = signature is valid; 0 = signature is invalid; other = and error occured)
	 */
	public function verify($data, $signature, $certificate){
		$pubKey = openssl_pkey_get_public($certificate->getCert(1));
		return openssl_verify($data, $signature, $pubKey);
	}
}

/*
// Class tests
$x509CA = new X509Certificate();
$x509CA->loadCert("UserCert.cer");
$pCA = new PrivateKey();
$pCA->load("UserPKey.pem");

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
XPTO 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345 TEST 12345
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

$sig = new RSADSig();
$sign = $sig->sign($testStr, $pCA);

print("signature = ".$sign);

// this should pass
$result = $sig->verify($testStr, $sign, $x509CA);
print("result = ".$result);

// this should fail
$result = $sig->verify($testStr2, $sign, $x509CA);
print("result = ".$result);
*/
?>