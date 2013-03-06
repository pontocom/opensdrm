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
 * X509Certificate.class.php
 *
 * This file contains a class that is used to create and manipulate X509 certificates
 * @author Carlos Serrão <carlos.j.serrao@gmail.com>
 * @version 1.0
 */

/**
* These are the required files for the class.
*/

require_once("config.inc.php");
require_once("PrivateKey.class.php");

/**
 * This is the definition of the X509Certificate class. This class will be used to create and manipulate X509 certificates
 *
 */
class X509Certificate
{
	/**
	 * The X509 certificate in resource format
	 *
	 * @var resource
	 * @access protected
	 */
	protected  $_x509certificate;
	/**
	 * The X509 certificate in string format
	 *
	 * @var string
	 * @access protected
	 */
	protected  $_sx509certificate;
	
	
	public $_csr;
	public $_scsr;

	/**
	 * The default class constructor
	 *
	 */
	function __construct()
	{
		$_x509certificate = null;
		$_sx509certificate = "";
		$_csr = null;
		$_scsr = "";
	}

	/**
	 * The default class destructor
	 *
	 */
	function __destruct(){
	}

	/**
	 * This function is used to create an X509 certificate
	 *
	 * @param X509Certificate $CAX509Certificate This is the X509 certificate of the entity that will issue the certificate. If this is a self-signed certificate this parameter can be null
	 * @param array $dn_data This is an associative array containing the DN directives that will be placed inside the certificate
	 * @param PrivateKey $SubjectPKey This is the subject private key - in the case this is a self signed certificate
	 * @param PrivateKey $CAPKey This is the CA private key - in the case this is a certificate signed by a CA
	 * @param int $days This corresponds to the number of days that a certificate is valid for
	 * @param string $passphrase This is an optional passphrase used to protect the certificate
	 */
	public function createCert($csr, $CAX509Certificate, $SubjectPKey, $CAPKey, $days, $passphrase=null){
		
		/*
		print("subjectpKey =\n");
		var_dump($SubjectPKey->_pKey);
		var_dump($SubjectPKey->_spKey);

		print("CAPkey =\n");
		var_dump($CAPKey->_pKey);
		var_dump($CAPKey->_spKey);

		print("CAX509Certificate=\n");
		var_dump($CAX509Certificate);
		var_dump($CAX509Certificate->_x509certificate);
		
		var_dump($dn_data);
		*/

		// this function creates a certificate
		// $csr = openssl_csr_new($dn_data, $SubjectPKey->_pKey, $GLOBALS['configargs']);

		//print("CSR = \n");
		//var_dump($csr);
		

		// either a self-signed certificate
		if($CAPKey == null)
			$this->_x509certificate = openssl_csr_sign($csr, $CAX509Certificate->_x509certificate, $SubjectPKey->_pKey, $days, $GLOBALS['configargs']);
		else // or a certificate signed by a CA
			$this->_x509certificate = openssl_csr_sign($csr, $CAX509Certificate->_x509certificate, $CAPKey->_pKey, $days, $GLOBALS['configargs']);

		//var_dump($this->_x509certificate);

		// we also store the certificate in a string readable format
		openssl_x509_export($this->_x509certificate, $this->_sx509certificate);
		
		//var_dump($this->_sx509certificate);
	}
	
	public function createCSR($dn_data, $SubjectPKey)
	{
		$this->_csr = openssl_csr_new($dn_data, $SubjectPKey->_pKey, $GLOBALS['configargs']);
		openssl_csr_export($this->_csr, $this->_scsr);
	}
	
	public function set($cert)
	{
		$this->_sx509certificate = $cert;
		$this->_x509certificate = openssl_x509_read($cert);
	}

	/**
	 * This function is used to return the certificate formated in a specific format
	 *
	 * @param int $format This is the format that the certificate should be returned. If null, the certificate is returned in resource format, if it's not null the certificate is returned in string format
	 * @return resource | string The certificate in the selected format
	 */
	public function getCert($format=null)
	{
		// returns the certificate either in string format or not
		if($format==null)
			return $this->_x509certificate;
		else
			return $this->_sx509certificate;
	}

	/**
	 * This function stores the certificate in a specific location
	 *
	 * @param string $location The location where the certificate will be stored
	 */
	public function storeCert($location)
	{
		// stores the certificate in a file
		openssl_x509_export_to_file($this->_x509certificate, $location);
	}

	/**
	 * This function is used to store the key - it is not used anymore.
	 *
	 */
	public function storeKey($key, $location)
	{
		openssl_pkey_export_to_file($key, $location, null, $GLOBALS['configargs']);
	}

	/**
	 * This function is used to load a certificate from a specific location
	 *
	 * @param string $location This indicates the location where the certificate is
	 */
	public function loadCert($location)
	{
			// we will read the certificate from a file
			$fp = fopen($location, "r");
			if($fp==null) throw new X509CertificateException("Cannot open the certificate file.");
			
			$this->_sx509certificate = fread($fp, 10000);
			fclose($fp);

			// to use this internally we need to have this as a openssl type
			$this->_x509certificate = openssl_x509_read($this->_sx509certificate);
	}

	/**
	 * This function parses the current certificate and returns some parsed information about it
	 *
	 * @return array Contains an array with the parsed fields of the certificate
	 */
	public function parseCert()
	{
		// we will parse a certificate and return an array with information
		return openssl_x509_parse($this->_sx509certificate);
	}

}

class X509CertificateException extends Exception
{
   // Redefine the exception so message isn't optional
   public function __construct($message, $code = 0) {
       // some code
   
       // make sure everything is assigned properly
       parent::__construct($message, $code);
   }

   // custom string representation of object
   public function __toString() {
       return __CLASS__ . ": [{$this->code}]: {$this->message}\n";
   }
}

// Class tests
/*
$x509CA = new X509Certificate();
$x509CA->loadCert("AUSCert.cer");
print_r($x509CA->parseCert());
*/

/*
// we need to read the private key from the CA
$pCA = new PrivateKey();
$pCA->load("AUSKeys.pem");
//var_dump($pCA->_spKey);

$pUser = new PrivateKey();
$pUser->create();

//var_dump($privkeyUser);
$dnUser = array("countryName" => 'PT', "stateOrProvinceName" => 'Lisboa', "localityName" => 'Lisboa', "organizationName" => 'ISCTE', "organizationalUnitName" => 'ADETTI', "commonName" => 'Carlos Serrao', "emailAddress" => 'carlos.serrao@iscte.pt');
$x509User = new X509Certificate();
//var_dump($x509CA->getCert(1));
$x509User->createCert($x509CA, $dnUser, $pUser, $pCA, 365, null);

var_dump($x509User->getCert(1));
var_dump($x509User->parseCert());

$pUser->save("UserPKey.pem");
$x509User->storeCert("UserCert.cer");
*/

?>