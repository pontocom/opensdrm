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

include_once("../openssl/X509Certificate.class.php");
include_once("../openssl/PrivateKey.class.php");
include_once("../openssl/RSADSig.class.php");

/**
 * This is used to implement the W3C XML Digital Signature standard - at least 
 * a partial implementation of it
 *
 */
class XMLDSig
{
    /*
    <Signature>
      <SignedInfo>
        (CanonicalizationMethod)
        (SignatureMethod)
        (<Reference (URI=)? >
          (Transforms)?
          (DigestMethod)
          (DigestValue)
        <Reference>)+
      </SignedInfo>
      (SignatureValue)
      (KeyInfo)?
      (Object)*
    </Signature>
    */
    
    /*
    <?xml version="1.0" encoding="UTF-8"?>
    <Signature xmlns="http://www.w3.org/2000/09/xmldsig#">
    <SignedInfo Id="foobar">
    <CanonicalizationMethod 
    Algorithm="http://www.w3.org/TR/2001/REC-xml-c14n-20010315"/>
    <SignatureMethod 
    Algorithm="http://www.w3.org/2000/09/xmldsig#dsa-sha1" /> 
    <Reference URI="http://www.abccompany.com/news/2000/03_27_00.htm">
    <DigestMethod Algorithm="http://www.w3.org/2000/09/xmldsig#sha1" />
    <DigestValue>j6lwx3rvEPO0vKtMup4NbeVu8nk=</DigestValue> 
    </Reference>
    <Reference URI="http://www.w3.org/TR/2000/WD-xmldsig-core-20000228/signature-example.xml">
    <DigestMethod Algorithm="http://www.w3.org/2000/09/xmldsig#sha1"/>
    <DigestValue>UrXLDLBIta6skoV5/A8Q38GEw44=</DigestValue> 
    </Reference>
    </SignedInfo>
    <SignatureValue>MC0E~LE=</SignatureValue>
    <KeyInfo>
    <X509Data>
    <X509SubjectName>CN=Ed Simon,O=XMLSec Inc.,ST=OTTAWA,C=CA</X509SubjectName>
    <X509Certificate>
    MIID5jCCA0+gA...lVN
    </X509Certificate>
    </X509Data>
    </KeyInfo>
    
    </Signature>
    */
    
    /**
     * This is the default constructor
     *
     */
    function __construct()
    {
        
    }
    
    /**
     * This is the default destructor
     *
     */
    function __destruct()
    {
        
    }
    
    /**
     * This function is used to generate a digital signature of a xml document
     *
     * @param string $xmldata
     * @param PrivateKey $prvKey
     * @param X509Certificate $x509cert
     * @return string
     */
    public function XMLsign($xmldata, $prvKey, $x509cert)
    {
        if($xmldata=="") throw new XMLDSigException("The XML data cannot be null.");
        
        $digestValue = base64_encode(sha1($xmldata, true));
        
        $sig = new RSADSig();
        $signatureValue = base64_encode($sig->sign($xmldata, $prvKey));
        
        $pX509 = $x509cert->parseCert();
        $subjectName = $pX509["name"];
        
        $certificate = $x509cert->getCert(1);
        
        $xmlsigvalue = 
        "<Signature xmlns='http://www.w3.org/2000/09/xmldsig#'>
           <SignedInfo Id='OpenSDRM-DSIG'>
             <CanonicalizationMethod Algorithm='http://www.w3.org/TR/2001/REC-xml-c14n-20010315'/>
             <SignatureMethod Algorithm='http://www.w3.org/2000/09/xmldsig#rsa-sha1' /> 
             <Reference>
               <Transforms>
                 <Transform Algorithm='http://www.w3.org/2000/02/xmldsig#base64' />
               </Transforms>
               <DigestMethod Algorithm='http://www.w3.org/2000/09/xmldsig#sha1' />
               <DigestValue>".$digestValue."</DigestValue> 
             </Reference>
           </SignedInfo>
           <SignatureValue>".$signatureValue."</SignatureValue>
           <KeyInfo>
             <X509Data>
               <X509SubjectName>".$subjectName."</X509SubjectName>
               <X509Certificate>".$certificate."</X509Certificate>
             </X509Data>
           </KeyInfo>
         </Signature>";
        
        return $xmlsigvalue;
    }
    
    public function XMLverify($full_xml_data)
    {
        if($full_xml_data=="") throw new XMLDSigException("The XML data cannot be null.");
        
        $xml = simplexml_load_string($full_xml_data);
        
        $certificate_data = $xml->KeyInfo[0]->X509Data[0]->X509Certificate;
        if($certificate_data==null) throw new XMLDSigException("Unable to retrieve the X509 certificate data.");
        
        $sigValue = $xml->SignatureValue;
        if($sigValue==null) throw new XMLDSigException("Unable to retrieve the signature value from the data.");
        
        return $certificate_data;    
    }
}

class XMLDSigException extends Exception
{
   // Redefine the exception so message isn't optional
   public function __construct($message, $code = 0) {
       // some code
   
       // make sure everything is assigned properly
       parent::__construct($message, $code);
   }

   // custom string representation of object
   public function __toString() {
       return __FILE__ . __CLASS__ . ": [{$this->code}]: {$this->message}\n";
   }
}


$x509User = new X509Certificate();
$x509User->loadCert("../openssl/UserCert.cer");
$pUser = new PrivateKey();
$pUser->load("../openssl/UserPKey.pem");

$xmldsig = new XMLDSig();
$v = $xmldsig->XMLsign("131212414", $pUser, $x509User);
var_dump($v);

$z=$xmldsig->XMLverify($v);
var_dump($z);
?>