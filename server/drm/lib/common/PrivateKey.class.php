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
/** PrivateKey class
To handle XML private keys in the following format:
<privateKey>
	<algorithmIdentifier>RSA-1024</algorithmIdentifier>
	<component>[n-value]</component>
	<component>[e-value]</component>
	<component>[d-value]</component>
	<component>[p-value]</component>
	<component>[q-value]</component>
</privateKey>
*/
class PrivateKey
{
	var $privkey; /**< a php array containing the private key */

	/**
	 * Constructor function of the PrivateKey.
	 * It does nothing.
	 */
	function PrivateKey()
	{
	}

	/**
	 * This sets the values of the private key.
	 * @param algid The identification of the PK algorithm. Default value is RSA.
	 * @param nc The RSA n value.
	 * @param ec The RSA e value.
	 * @param ec The RSA d value.
	 * @param ec The RSA p value.
	 * @param ec The RSA q value.
	 */
	function set($algid="RSA", $nc, $ec, $dc, $pc, $qc){
		$this->bundle($algid, $nc, $ec, $dc, $pc, $qc);
	}

	/**
	 * Bundles all the private key values in one array.
	 * @param algid The identification of the PK algorithm. Default value is RSA.
	 * @param nc The RSA n value.
	 * @param ec The RSA e value.
	 * @param ec The RSA d value.
	 * @param ec The RSA p value.
	 * @param ec The RSA q value.
	 */
	 function bundle($algid="RSA", $nc, $ec, $dc, $pc, $qc){
		$this->privkey = array(
			"algorithm_id" => $algid,
			"n" => $nc,
			"e" => $ec,
			"d" => $dc,
			"p" => $pc,
			"q" => $qc);
	}

	/**
	 * Returns the Algorithm ID use to create this private key.
	 * @return Algorithm ID
	 */
	function getAlgId()
	{
		return $this->privkey["algorithm_id"];
	}

	/**
	 * Returns the RSA N value of the private key
	 * @return RSA N value
	 */
	function getN()
	{
		return $this->privkey["n"];
	}

	/**
	 * Returns the RSA E value of the private key
	 * @return RSA E value
	 */
	function getE()
	{
		return $this->privkey["e"];
	}

	/**
	 * Returns the RSA D value of the private key
	 * @return RSA D value
	 */
	function getD()
	{
		return $this->privkey["d"];
	}

	/**
	 * Returns the RSA P value of the private key
	 * @return RSA P value
	 */
	function getP()
	{
		return $this->privkey["p"];
	}

	/**
	 * Returns the RSA Q value of the private key
	 * @return RSA Q value
	 */
	function getQ()
	{
		return $this->privkey["q"];
	}

	/**
	 * Returns all the values of the RSA private key
	 * @return The array value of the private key
	 */
	function getAll()
	{
		return $this->privkey;
	}

	/**
	 * Reads the XML data and puts it on a PHP array
	 * @param xmldata The XML to be stored on the array
	 */
	function loadXMLkey($xmldata) {
		$xmla = xmlize($xmldata);
		$this->privkey = array(
			"algorithm_id" => $xmla["privateKey"]["#"]["algorithmIdentifier"][0]["#"],
			"n" => $xmla["privateKey"]["#"]["component"][0]["#"],
			"e" => $xmla["privateKey"]["#"]["component"][1]["#"],
			"d" => $xmla["privateKey"]["#"]["component"][2]["#"],
			"p" => $xmla["privateKey"]["#"]["component"][3]["#"],
			"q" => $xmla["privateKey"]["#"]["component"][4]["#"]);
	}

	/**
	 * Returns the private key stored in array in XML format
	 * @return Private Key in XML format.
	 */
	function returnXMLkey() {
		return "<privateKey><algorithmIdentifier>".$this->privkey["algorithm_id"]."</algorithmIdentifier><component>".$this->privkey["n"]."</component><component>".$this->privkey["e"]."</component><component>".$this->privkey["d"]."</component><component>".$this->privkey["p"]."</component><component>".$this->privkey["q"]."</component></privateKey>";
	}

}