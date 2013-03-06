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
 * This is a basic class to cipher and decipher data using the AES algorithm (the key lenght is fixed with 128, 192 or 256 bits)
 *
 * @author Carlos Serrão <carlos.j.serrao@gmail.com>
 * @version 1.0
 */
class AESEncrypt
{
    /**
     * This variable holds the key used to cipher the content
     *
     * @access public
     * @var string
     */
    public $key;
    /**
     * Holds the Initialization Vector (IV)
     *
     * @access public
     * @var string
     */
    public $IV;
    /**
     * The cipher mode in which the algorithm works: CBC, ECB, CFB, OFB.
     *
     * @access public
     * @var string
     */
    public $mode;
    
    /**
     * This is the constructor function of the class. It initializes the key, the IV and the cipher mode
     *
     * @param string $_key The key
     * @param string $_IV The IV
     * @param string $_mode The cipher mode to be used
     * @throws AESEncryptException
     */
    function __construct($_key, $_IV, $_mode)
    {
        $this->key = $_key;
        $this->IV = $_IV;
        $this->mode = $_mode;
        
        if(strlen($this->IV) * 8 != 128)
            throw new AESEncryptException("Wrong IV size - should be 128 bits, 16 bytes");
            
        if($this->mode!="CBC" && $this->mode!="EBC" && $this->mode!="CFB" && $this->mode!="OFB") 
            throw new AESEncryptException("Invalid cipher mode or not supported.");
    }
    
    /**
     * This is the default destructor of the class
     *
     */
    function __destruct()
    {
    }
    
    /**
     * This function is used to cipher data with the key, IV and cipher mode selected
     *
     * @param string $data_to_encrypt This is the data to be ciphered.
     * @return string The encrypted data, with or without padding depending on the cipher mode selected
     * @throws AESEncryptException
     */
	public function AES_encrypt($data_to_encrypt) {
	    /* check the key size */
	    $ks = strlen($this->key) * 8;
	    switch($ks)
	    {
	        case 128: $alg = MCRYPT_RIJNDAEL_128; break;
	        case 192: $alg = MCRYPT_RIJNDAEL_192; break;
	        case 256: $alg = MCRYPT_RIJNDAEL_192; break;
	        default: throw new AESEncryptException("Wrong key size", 1); break;
	    }
	    
	    
	    switch ($this->mode) {
	        // adds padding
	        case "CBC": return mcrypt_cbc($alg, $this->key, $data_to_encrypt, MCRYPT_ENCRYPT, $this->IV);
	            break;
	        // adds padding
	        case "ECB": return mcrypt_ecb($alg, $this->key, $data_to_encrypt, MCRYPT_ENCRYPT, $this->IV);
	            break;
	        // no padding
	        case "CFB": return mcrypt_cfb($alg, $this->key, $data_to_encrypt, MCRYPT_ENCRYPT, $this->IV);
	           break;
	        // no padding
	        case "OFB": return mcrypt_ofb($alg, $this->key, $data_to_encrypt, MCRYPT_ENCRYPT, $this->IV);
	           break;
	        // adds padding
	        default: return mcrypt_cbc($alg,$this->key, $data_to_encrypt, MCRYPT_ENCRYPT, $this->IV);
	           break;
	    }
	    return null;
	}

	/**
	 * This function is used to decipher data with the key, IV and cipher mode selected
	 *
	 * @param string $data_to_decrypt This is the data to be dciphered.
	 * @return string The decrypted data.
	 * @throws AESEncryptException
	 */
	public function AES_decrypt($data_to_decrypt) {
	    /* check the key size */
	    $ks = strlen($this->key) * 8;
	    switch($ks)
	    {
	        case 128: $alg = MCRYPT_RIJNDAEL_128; break;
	        case 192: $alg = MCRYPT_RIJNDAEL_192; break;
	        case 256: $alg = MCRYPT_RIJNDAEL_192; break;
	        default: throw new AESEncryptException("Wrong key size", 1); break;
	    }

	    switch ($this->mode) {
	        // adds padding
	        case "CBC": return mcrypt_cbc($alg, $this->key, $data_to_decrypt, MCRYPT_DECRYPT, $this->IV);
	            break;
	        // adds padding
	        case "ECB": return mcrypt_ecb($alg, $this->key, $data_to_decrypt, MCRYPT_DECRYPT, $this->IV);
	            break;
	        // no padding
	        case "CFB": return mcrypt_cfb($alg, $this->key, $data_to_decrypt, MCRYPT_DECRYPT, $this->IV);
	           break;
	        // no padding
	        case "OFB": return mcrypt_ofb($alg, $this->key, $data_to_decrypt, MCRYPT_DECRYPT, $this->IV);
	           break;
	        // adds padding
	        default: return mcrypt_cbc($alg,$this->key, $data_to_decrypt, MCRYPT_DECRYPT, $this->IV);
	           break;
	    }
	    return null;
	}

	/**
	 * Performs base64 encodings and decodings over data
	 *
	 * @param string $data This is the data to encode or decode.
	 * @param string $op This is the operation to perform: B64ENC or B64DEC
	 * @return string This is the data coded or decoded in Base64.
	 */
	public function opb64($data, $op)
	{
	    switch ($op)
	    {
	        case "B64ENC": return base64_encode($data);
	           break;
	        case "B64DEC": return base64_decode($data);
	           break;
	    }
	}
}

/**
 * An Exception class to handle the Exceptions of AESEncrypt
 *
 */
class AESEncryptException extends Exception
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
try {
    $aes = new AESEncrypt("11111111111111111111111111111111", "xxxx111111111111", "OFB");
    $cipher = $aes->AES_encrypt("o meu nome");
    var_dump($cipher);
    $decipher = $aes->AES_decrypt($cipher);
    var_dump($decipher);
} catch (AESEncryptException $e)
{
    echo $e;
}
*/
?>