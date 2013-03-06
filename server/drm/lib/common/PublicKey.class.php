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

include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/xmlize.inc.php");
/** PublicKey class
To handle XML private keys in the following format:
<publicKey>
	<algorithmIdentifier>RSA-1024</algorithmIdentifier>
	<component>[e-value]</component>
	<component>[n-value]</component>
</publicKey>
*/
class PublicKey
{
	var $pubkey; /**< a php array containing the public key */

	/**
	 * Constructor function of the PublicKey.
	 * It does nothing.
	 */
	function PublicKey()
	{
	}

	/**
	 * This sets the values of the private key.
	 * @param algid The identification of the PublicKey algorithm. Default value is RSA.
	 * @param nc The RSA 'n' value.
	 * @param ec The RSA 'e' value.
	 */
	function set($algid="RSA", $nc, $ec){
		$this->bundle($algid, $nc, $ec);
	}

	/**
	 * Bundles together the initials values of the public-key.
	 * @param algid The identification of the PublicKey algorithm. Default value is RSA.
	 * @param nc The RSA n value.
	 * @param ec The RSA e value.
	 */
	 function bundle($algid="RSA", $nc, $ec){
		$this->pubkey = array(
			"algorithm_id" => $algid,
			"n" => $nc,
			"e" => $ec);
	}

	/**
	 * Returns the Algorithm Identifier.
	 * @return Algorithm ID
	 */
	function getAlgId()
	{
		return $this->pubkey["algorithm_id"];
	}

	/**
	 * Returns the RSA N value of the public key
	 * @return RSA N value
	 */
	function getN()
	{
		return $this->pubkey["n"];
	}

	/**
	 * Returns the RSA E value of the public key
	 * @return RSA E value
	 */
	function getE()
	{
		return $this->pubkey["e"];
	}

	/**
	 * Returns all the values of the RSA public key
	 * @return The array value of the public key
	 */
	function getAll()
	{
		return $this->pubkey;
	}

	/**
	 * Returns the hash of RSA public key
	 * @return The hash of the value of the public key
	 */
	function getHashOfKey() {
		$tb_hashed = "".$this->pubkey["n"].$this->pubkey["e"];
		return md5($tb_hashed);
	}

	/**
	 * Reads the XML data and puts it on a PHP array
	 * @param xmldata The XML to be stored on the array
	 */
	function loadXMLkey($xmldata) {
		$xmla = xmlize($xmldata);
		$this->pubkey = array(
			"algorithm_id" => $xmla["publicKey"]["#"]["algorithmIdentifier"][0]["#"],
			"n" => $xmla["publicKey"]["#"]["component"][0]["#"],
			"e" => $xmla["publicKey"]["#"]["component"][1]["#"]);
	}

	/**
	 * Returns the ppublic key stored in array in XML format
	 * @return Public Key in XML format.
	 */
	function returnXMLkey() {
		return "<publicKey><algorithmIdentifier>".$this->pubkey["algorithm_id"]."</algorithmIdentifier><component>".$this->pubkey["n"]."</component><component>".$this->pubkey["e"]."</component></publicKey>";
	}
}