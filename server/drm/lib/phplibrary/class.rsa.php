<?
///////////////////////////////////////////////////////////////////////////////
// Filename: class.rsa.php
// Date: 23.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

class CRSA
{
	var $p, $q, $n, $e, $d;

	function CRSA()
	{
		
		/*
		Small Random Values

		p = 6 Digits
		q = 6 Digits
		n = 12 Digits
		*/

		/*
		$this->p = '735373';
		$this->q = '861299';
		$this->n = '633376029527';
		$this->e = '5';
		$this->d = '190012329857';
		*/

		/*
		Large Random Values

		p = 156 Digits
		q = 156 Digits
		n = 311 Digits
		*/

		//$this->p = '112122319636296693236328719169899838626328749276497698636428964892764987628947284689767896198627631362193628917631876861638713612863128736871367617231624021';
		//$this->q = '230894773429857093247592384750894478907890078910789127890347089123470894230478123970148923780491237048923107489237048923702431071248930427891302479138079339';
		//$this->n = '25888457588852741952488701308748538874089526050672952264258339115744304723418792934329113460307935536759676864246123133627758090849960267882953064728302994798904546591622037492292909029328108154021848243886870818415342910823139324658184437124585518989851994210609706251488247515828591268959942726595713716202119';
		//$this->e = '7';
		//$this->d = '11095053252365460836780871989463659517466939736002693827539288192461844881465196971855334340131972372897004370391195628697610610364268686235551313454986997623951765796629250432159345046514481694201294309971447553559695240537396056528332475946185231373278678468802842592526769503435960210100333413110978862785183';
	}

	function CreatePrimes($sSeed, $iBits = 1024)
	{
		$cRandom = New CRandom;		

		$cRandom->Seed($sSeed);

		$this->p = SearchPrime($cRandom->Random($iBits / 2));
		$this->q = SearchPrime($cRandom->Random($iBits / 2));

		if(MathComp($this->p, $this->q) > 0)
			Swap($this->p, $this->q);
	}

	function GetPrimes()
	{
		$aPrimes['p'] = $this->p;
		$aPrimes['q'] = $this->q;

		return $aPrimes;
	}

	function SetPrimes($aPrimes)
	{
		$this->p = $aPrimes['p'];
		$this->q = $aPrimes['q'];
	}

	function CalcKeys()
	{
		RSAKeys($this->p, $this->q, $this->n, $this->e, $this->d);
	}

	function TestKeys($sMessage = 'MyMessage4TestKeys')
	{
		return ($sMessage == $this->Decrypt($this->Encrypt($sMessage)));
	}

	function SetPublicKey($aPublicKey)
	{
		$this->n = $aPublicKey['n'];
		$this->e = $aPublicKey['e'];
	}

	function GetPublicKey()
	{
		$aPublicKey['n'] = $this->n;
		$aPublicKey['e'] = $this->e;

		return $aPublicKey;
	}

	function SetPrivateKey($aPrivateKey)
	{
		$this->n = $aPrivateKey['n'];
		$this->d = $aPrivateKey['d'];
	}

	function GetPrivateKey()
	{
		$aPrivateKey['n'] = $this->n;
		$aPrivateKey['d'] = $this->d;

		return $aPrivateKey;
	}

	/*
	e.g.
	n = 1094861636 => ABCD => StrLen => 4

	n = 16777216 (0x100000)
	n - 1 = 16777215 (0xffffff)
	Len(I2OS(n - 1)) = 3

	*/
	function BlockSize()
	{
		return StrLen(MathDecToStr(MathSub($this->n, 1)));
	}

	function Encrypt($sMessage)
	{
		$iBlockSizeSource = $this->BlockSize() - 1;
		$iBlockSizeDestination = $iBlockSizeSource + 1;

		for($i = 0; $i < StrLen($sMessage); $i += $iBlockSizeSource)
			$aChunk[] = SubStr($sMessage, $i, $iBlockSizeSource);

		for($i = 0; $i < Count($aChunk); $i++)
			$aChunk[$i] = MathDecToStr(RSAE($this->n, $this->e, MathStrToDec($aChunk[$i])), $iBlockSizeDestination);

		return Implode('', $aChunk);
	}

	function Decrypt($sMessage)
	{
		$iBlockSizeSource = $this->BlockSize() - 1;
		$iBlockSizeDestination = $iBlockSizeSource + 1;

		for($i = 0; $i < StrLen($sMessage); $i += $iBlockSizeDestination)
			$aChunk[] = SubStr($sMessage, $i, $iBlockSizeDestination);

		for($i = 0; $i < Count($aChunk); $i++)
			$aChunk[$i] = MathDecToStr(RSAD($this->n, $this->d, MathStrToDec($aChunk[$i])), $iBlockSizeSource);

		return Implode('', $aChunk);
	}

	function Hash($sMessage)
	{
		return MD5($sMessage) . CRC32($sMessage);
	}

	function Signature($sMessage)
	{
		$m = MathMod(MathStrToDec($this->Hash($sMessage)), $this->n);
		return MathDecToStr(RSAS($this->n, $this->d, $m));
	}

	function Verification($sMessage, $sSignature)
	{
		$s = MathStrToDec($sSignature);
		return MathMod(MathStrToDec($this->Hash($sMessage)), $this->n) == RSAV($this->n, $this->e, $s);
	}
	
}

/*
$sOriginalMessage = 'MyMessage';

$aPublicKey = array('n' => '218134251288113034191732007174369784441', 'e' => '5');
$cPublicRSA = new CRSA;
$cPublicRSA->SetPublicKey($aPublicKey);
$sCipherText = Base64_Encode($cPublicRSA->Encrypt($sOriginalMessage));

$aPrivateKey = array('n' => '218134251288113034191732007174369784441', 'd' => '2423713903201255935132839554520483207');
$cPrivateRSA = new CRSA;
$cPrivateRSA->SetPrivateKey($aPrivateKey);
$sMessage = $cPrivateRSA->Decrypt(Base64_Decode($sCipherText));

W($sOriginalMessage == $sMessage);
*/

function TestClassRSA()
{
	$aTests[] = '$c = new CRSA; $c->CreatePrimes("9876543210", 1024); $c->CalcKeys(); return $c->TestKeys();';
	//$aTests[] = '$c = new CRSA; $c->CreatePrimes("9876543210", 64); $c->CalcKeys(); return $c->n == "6209396064698498741";';
	//$aTests[] = '$c = new CRSA; $c->CreatePrimes("9876543210", 64); $c->CalcKeys(); return MathStrToHex($c->Encrypt("MyMessage")) == "488c30707105ad3750b046ae327380a6";';
	//$aTests[] = '$c = new CRSA; $c->CreatePrimes("9876543210", 64); $c->CalcKeys(); return $c->Decrypt(MathHexToStr("488c30707105ad3750b046ae327380a6")) == "MyMessage";';
	//$aTests[] = '$c = new CRSA; $c->CreatePrimes("9876543210", 64); $c->CalcKeys(); return MathStrToHex($c->Signature("MyMessage")) == "26e5516a1897bc19";';
	//$aTests[] = '$c = new CRSA; $c->CreatePrimes("9876543210", 64); $c->CalcKeys(); return $c->Verification("MyMessage", MathHexToStr("26e5516a1897bc19"));';
	
	return Test('ClassRSA', $aTests);
}

?>