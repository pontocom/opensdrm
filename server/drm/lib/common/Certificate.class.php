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
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"].'/common/xmlize.inc.php');
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"].'/common/RSASignature.class.php');
/** Certificate class
To handle XML certificates in the following format:

<certificate>
	<issuerData>
		<identification>[hash-value-of-public-key]</identification>
		<signatureAlgorithmIdentifier>RSA_WITH_MD5</signatureAlgorithmIdentifier>
		<publicKey>
			<algorithmIdentifier>RSA-1024</algorithmIdentifier>
			<component>[n-value]</component>
			<component>[e-value]</component>
		</publicKey>
	</issuerData>
	<subjectData>
		<identification>[XNS, Liberty Alliance, or other data]</identification>
		<publicKey>
			<algorithmIdentifier>RSA-1024</algorithmIdentifier>
			<component>[n-value]</component>
			<component>[e-value]</component>
		</publicKey>
	</subjectData>
	<validity>
		<notBefore>[date and time]</notBefore>
		<notAfter>[date and time]</notAfter>
	</validity>
	<signature>[signature-value]</signature>
</certificate>
*/

class Certificate
{
	/**
	 * $certificate variable containing a PHP array containing certificate
	 */
	var $certificate; /**< a php array containing the certificate */
	/*
	certificate array (
		issuerData array (
			identification,
			signatureAlgorithm,
			publicKey array (
				algorithmIdentifier,
				component,
				component
			)
		),
		subjectData array (
			identification
			publicKey array (
				algorithmIdentifier,
				component,
				component
			)
		),
		validity array (
			notBefore,
			notAfter
		),
		signature
	)
	*/

	/**
	 * Constructor function of the Certificate.
	 * It does nothing.
	 */
	function Certificate()
	{
	}

	/**
	* This function loads in the internal array a XML formated certificate.
	* @param xml_data A variable containing the XML data of the certificate
	* @return Nothing.
	*/
	function loadXMLcertificate($xml_data) {
		$xmla = xmlize($xml_data);

		$this->certificate = array (
			"issuerData" => array (
				"identification"=> $xmla["certificate"]["#"]["issuerData"][0]["#"]["identification"][0]["#"],
				"signatureAlgorithmIdentifier" => $xmla["certificate"]["#"]["issuerData"][0]["#"]["signatureAlgorithmIdentifier"][0]["#"],
				"publicKey" => array (
					"algorithmIdentifier" => $xmla["certificate"]["#"]["issuerData"][0]["#"]["publicKey"][0]["#"]["algorithmIdentifier"][0]["#"],
					"n" => $xmla["certificate"]["#"]["issuerData"][0]["#"]["publicKey"][0]["#"]["component"][0]["#"],
					"e" => $xmla["certificate"]["#"]["issuerData"][0]["#"]["publicKey"][0]["#"]["component"][1]["#"]
				),
			),
			"subjectData" => array (
				"identification"=> $xmla["certificate"]["#"]["subjectData"][0]["#"]["identification"][0]["#"],
				"publicKey" => array (
					"algorithmIdentifier" => $xmla["certificate"]["#"]["subjectData"][0]["#"]["publicKey"][0]["#"]["algorithmIdentifier"][0]["#"],
					"n" => $xmla["certificate"]["#"]["subjectData"][0]["#"]["publicKey"][0]["#"]["component"][0]["#"],
					"e" => $xmla["certificate"]["#"]["subjectData"][0]["#"]["publicKey"][0]["#"]["component"][1]["#"]
				),
			),
			"validity" => array (
				"notBefore" => $xmla["certificate"]["#"]["validity"][0]["#"]["notBefore"][0]["#"],
				"notAfter" => $xmla["certificate"]["#"]["validity"][0]["#"]["notAfter"][0]["#"]
			),
			"signature" => $xmla["certificate"]["#"]["signature"][0]["#"]
		);
	}

	/**
	* This function returns the internal certificate array value in a XML encoded format
	* @return String containing the XML certificate.
	*/
	function returnXMLcertificate() {
		return "<certificate><issuerData><identification>".$this->certificate["issuerData"]["identification"]."</identification><signatureAlgorithmIdentifier>".$this->certificate["issuerData"]["signatureAlgorithmIdentifier"]."</signatureAlgorithmIdentifier><publicKey><algorithmIdentifier>".$this->certificate["issuerData"]["publicKey"]["algorithmIdentifier"]."</algorithmIdentifier><component>".$this->certificate["issuerData"]["publicKey"]["n"]."</component><component>".$this->certificate["issuerData"]["publicKey"]["e"]."</component></publicKey></issuerData><subjectData><identification>".$this->certificate["subjectData"]["identification"]."</identification><publicKey><algorithmIdentifier>".$this->certificate["subjectData"]["publicKey"]["algorithmIdentifier"]."</algorithmIdentifier><component>".$this->certificate["subjectData"]["publicKey"]["n"]."</component><component>".$this->certificate["subjectData"]["publicKey"]["e"]."</component></publicKey></subjectData><validity><notBefore>".$this->certificate["validity"]["notBefore"]."</notBefore><notAfter>".$this->certificate["validity"]["notAfter"]."</notAfter></validity><signature>".$this->certificate["signature"]."</signature></certificate>";
	}

	function createCertificateFrom($AUSpublickey, $AUSprivatekey, $uid, $pubkey)
	{
		$today = getdate();
        $timestamp = mktime($today[hours],$today[minutes],$today[seconds],$today[mon], $today[mday], $today[year]);
        $notbefore = sprintf("%02d:%02d:%02d-%04d%02d%02d",$today[hours],$today[minutes],$today[seconds],$today[year], $today[mon], $today[mday]);

        $newtimestamp = $timestamp+86400*365;

        $notafter = date("H:i:s-Ymd", $newtimestamp);

		$to_be_signed = "<issuerData><identification>".$AUSpublickey->getHashOfKey()."</identification><signatureAlgorithmIdentifier>RSA_WITH_MD5</signatureAlgorithmIdentifier><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>".$AUSpublickey->getN()."</component><component>".$AUSpublickey->getE()."</component></publicKey></issuerData><subjectData><identification>".$uid."</identification><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>".$pubkey->getN()."</component><component>".$pubkey->getE()."</component></publicKey></subjectData><validity><notBefore>".$notbefore."</notBefore><notAfter>".$notafter."</notAfter></validity>";

		$rsa_sign = new RSASignature();

		$cert_xml = "<certificate>".$to_be_signed."<signature>".$rsa_sign->RSA_sign($to_be_signed, $AUSprivatekey)."</signature></certificate>";

		$this->loadXMLcertificate($cert_xml);
	}

	function getIssuerPublicKey() {
		return "<publicKey><algorithmIdentifier>".$this->certificate["issuerData"]["publicKey"]["algorithmIdentifier"]."</algorithmIdentifier><component>".$this->certificate["issuerData"]["publicKey"]["n"]."</component><component>".$this->certificate["issuerData"]["publicKey"]["e"]."</component></publicKey>";
	}

	function getSubjectPublicKey() {
		return "<publicKey><algorithmIdentifier>".$this->certificate["subjectData"]["publicKey"]["algorithmIdentifier"]."</algorithmIdentifier><component>".$this->certificate["subjectData"]["publicKey"]["n"]."</component><component>".$this->certificate["subjectData"]["publicKey"]["e"]."</component></publicKey>";
	}

	function getCertificateBody() {
		return "<issuerData><identification>".$this->certificate["issuerData"]["identification"]."</identification><signatureAlgorithmIdentifier>".$this->certificate["issuerData"]["signatureAlgorithmIdentifier"]."</signatureAlgorithmIdentifier><publicKey><algorithmIdentifier>".$this->certificate["issuerData"]["publicKey"]["algorithmIdentifier"]."</algorithmIdentifier><component>".$this->certificate["issuerData"]["publicKey"]["n"]."</component><component>".$this->certificate["issuerData"]["publicKey"]["e"]."</component></publicKey></issuerData><subjectData><identification>".$this->certificate["subjectData"]["identification"]."</identification><publicKey><algorithmIdentifier>".$this->certificate["subjectData"]["publicKey"]["algorithmIdentifier"]."</algorithmIdentifier><component>".$this->certificate["subjectData"]["publicKey"]["n"]."</component><component>".$this->certificate["subjectData"]["publicKey"]["e"]."</component></publicKey></subjectData><validity><notBefore>".$this->certificate["validity"]["notBefore"]."</notBefore><notAfter>".$this->certificate["validity"]["notAfter"]."</notAfter></validity>";
	}

	function getCertificateSignature() {
		return $this->certificate["signature"];
	}

	function getCertificateIssuerId() {
		return $this->certificate["issuerData"]["identification"];
	}

	function getCertificateSubjectId() {
		return $this->certificate["subjectData"]["identification"];
	}
}