<?
///////////////////////////////////////////////////////////////////////////////
// Filename: class.random.php
// Date: 23.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

class CRandom
{
	var $m_cMersenneTwister;

	function CRandom()
	{
		$this->m_cMersenneTwister = New CMersenneTwister;
	}

	//2 ^ 32 = 4294967296
	function Seed($qNumber)
	{
		while($qNumber != '0')
		{
			$aInit[] = MathMod($qNumber, '4294967296');
			$qNumber = MathDiv($qNumber, '4294967296');
		}
		$this->m_cMersenneTwister->init_by_array($aInit);
	}

	function Random($iBits = 128)
	{
		$iDoubleWords = Ceil($iBits / 32);

		$qNumber = '0';
		for($i = 0; $i < $iDoubleWords; $i++)
			$qNumber = MathAdd($qNumber, MathShL($this->m_cMersenneTwister->genrand_int32(), $i * 32));

		$qNumber = MathMod($qNumber, MathShL(1, $iBits));

		return $qNumber;
	}

}

/*
Init

22420397091237097908709237049712049712093710897334294967296

Results

190278759957166172488589981608291776714
272150785021623620013017884490613651271
166804320519397484456537398052828630599
176175885926937919003480290979430677858
272934434638045391529356937184900277069
76924409475820866834908385058283701631
13102726585924243687928436892915079501
105709757713733217328245183891337765275
310642086071845818212085915094245902563
69310343423272790893948989007635616852
*/

function TestClassRandom()
{
	$aTests[] = '$cRandom = New CRandom; $cRandom->Seed("1234"); return $cRandom->Random(32) == "4150886329";';

	return Test('ClassRandom', $aTests);
}

?>