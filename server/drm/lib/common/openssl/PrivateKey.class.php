<?
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
 * PrivateKey.class.php
 *
 * This file contains a class that is used to manipulate a Private key
 * (from a RSA key pair)
 * @author Carlos Serrão <carlos.j.serrao@gmail.com>
 * @version 1.0
 */

/**
* These are the required files for the class.
*/

require_once("config.inc.php");

/**
 * This is the declaration of the PrivateKey class. This class will be used to hold the RSA keys and to perform several operations over these keys
 */
class opsslPrivateKey
{
	/**
	* The private key in a resource format
	* @access public
	* @var resource
	*/
	public $_pKey;

	/**
	* The private key in a string format
	* @access public
	* @var string
	*/
	public $_spKey;

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
	 * This function is used to create a new RSA key pair. The key will be created according to the parameters that are specified in the openssl configuration file
	 *
	 */
	public function create(){
		$this->_pKey = openssl_pkey_new($GLOBALS['configargs']);
		openssl_pkey_export($this->_pKey, $this->_spKey, null, $GLOBALS['configargs']);
	}

	/**
	 * This function is used to sign an arbitrary block of data using RSA (private key) and using the SHA-1 hasing algorithm
	 * @param string $format The format of the data to be returned - NULL returns the data in resource format, non-null returns the data in string format
	 * @return string | resource The key resturned in the selected format
	 */
	public function get($format=null){
		if ($format!=null)
			return $_pKey;
		else
			return $_spKey;
	}

	/**
	 * This function is used to set a key pair - not implemented yet!!!
	 *
	 */
	public function set($pkey)
	{
		$this->_spKey = $pkey;
		$this->_pKey = openssl_pkey_get_private($pkey);
	}

	/**
	 * This function is used to save a key-pair to a specific location
	 *
	 * @param string $location Indicates the location where the key pair will be saved.
	 */
	public function save($location){
		openssl_pkey_export_to_file($this->_spKey, $location, null, $GLOBALS['configargs']);
	}

	/**
	 * This functon is used to load a key-pair from a specific location
	 *
	 * @param string $location The location of the file containing the key-pair to be loaded.
	 */
	public function load($location){
		$fp = fopen($location, "r");
		if($fp==null) throw new opsslPrivateKeyException("Private key file not found.");
		
		$this->_spKey = fread($fp, 10000);
		fclose($fp);

		$this->_pKey = openssl_pkey_get_private($this->_spKey);
	}
}

class opsslPrivateKeyException extends Exception
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

/*
// Class tests
$pk = new PrivateKey();
$pk->create();
var_dump($pk->_pKey);
var_dump($pk->_spKey);
*/
?>