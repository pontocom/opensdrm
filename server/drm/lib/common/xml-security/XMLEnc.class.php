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

include_once("../mcrypt/AESEncrypt.class.php");

/**
 * This is a class that will encrypt XML data according to the XML data supplied
 * For now, this class will only support AES encryption and it is a very limited 
 * implementation.
 *
 * @author Carlos Serrão <carlos.j.serrao@gmail.com>
 * @version 1.0
 */
class XMLEnc
{
    /**
  <EncryptedData Id? Type? MimeType? Encoding?>
    <EncryptionMethod/>?
    <ds:KeyInfo>
      <EncryptedKey>?
      <AgreementMethod>?
      <ds:KeyName>?
      <ds:RetrievalMethod>?
      <ds:*>?
    </ds:KeyInfo>?
    <CipherData>
      <CipherValue>?
      <CipherReference URI?>?
    </CipherData>
    <EncryptionProperties>?
  </EncryptedData>
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
     * This is the function that is going to be used to encrypt XML data
     *
     * @param string $xml_data The XML data to be ciphered - just the part that is going to be ciphered
     * @param string $type This indicates what is the type of data to be ciphered: XMLELEMENT, XMLELEMENTCONTENTELEM, XMLELEMENTCONTENTDATA, ARBITRARY.
     * @param AESEncrypt $aes The AES encrypt object created previoulsy.
     * @return string Containing the ciphered data
     * @throws XMLEncException
     */
    public function encryptXML($xml_data, $type, $aes)
    {
        if($xml_data=='') throw new XMLEncException("Data cannot be empty");
        
        switch ($type)
        {
            case "XMLELEMENT":
                $start = "<EncryptedData xmlns='http://www.w3.org/2001/04/xmlenc#' Type='http://www.w3.org/2001/04/xmlenc#Element'>";
                break;
            case "XMLELEMENTCONTENTELEM":
                $start = "<EncryptedData xmlns='http://www.w3.org/2001/04/xmlenc#' Type='http://www.w3.org/2001/04/xmlenc#Content'>";
                break;
            case "XMLELEMENTCONTENTDATA":
                $start = "<EncryptedData xmlns='http://www.w3.org/2001/04/xmlenc#' Type='http://www.w3.org/2001/04/xmlenc#Content'>";
                break;
            case "ARBITRARY":
                $start = "<EncryptedData xmlns='http://www.w3.org/2001/04/xmlenc#' MimeType='text/xml'>";
                break;
            default: throw new XMLEncException("Type is not valid, or not implemented.");
        }
        
        $enc_method ="<EncryptionMethod Algorithm='http://www.w3.org/2001/04/xmlenc#aes128-cfb'/>";
        $cipher_data_begin ="<CipherData><CipherValue>";
        
        $xmlcipher = $aes->AES_encrypt($xml_data);
        
        $cipher_data_end = "</CipherValue></CipherData>";
        $end = "</EncryptedData>";
        
        $final = $start.$enc_method.$cipher_data_begin.$aes->opb64($xmlcipher, "B64ENC").$cipher_data_end.$end;
        
        return  $final;
    }
    
    /**
     * This function used to decipher XML data
     *
     * @param string $xml_data The data to be deciphered.
     * @param AESEncrypt $aes The AES encrypt object created previoulsy.
     * @return string The deciphered data.
     * @throws XMLEncException
     */
    public function decryptXML($xml_data, $aes)
    {
        if($xml_data=='') throw new XMLEncException("Data cannot be empty");

        $xml = simplexml_load_string($xml_data);
        
        $xmlcipher = $xml->CipherData[0]->CipherValue;
        
        $xmldecipher = $aes->AES_decrypt($aes->opb64($xmlcipher, "B64DEC"));
        
        return $xmldecipher;
    }
}

class XMLEncException extends Exception
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

/*
try {
    $aes = new AESEncrypt("1111111111111111", "xxxx111111111111", "OFB");
    $xmlenc = new XMLEnc();
    $data = $xmlenc->encryptXML("<xml><elemento>uma coisa qq</elemento></xml>", "XMLELEMENT", $aes);
    var_dump($data);
    $data2 = $xmlenc->decryptXML($data, $aes);
    var_dump($data2);
} catch (AESEncryptException $e)
{
    echo $e;
}
*/
?>