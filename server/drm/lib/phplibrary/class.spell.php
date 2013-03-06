<?
///////////////////////////////////////////////////////////////////////////////
// Filename: class.spell.php
// Date: 26.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

class CSpell
{
	var $m_aDictionary;

	function CSpell($sFileName, $sLanguage)
	{
		$aInputLines = GetFileAsArray($sFileName);
		$iColumn = Array_Search($sLanguage, Explode(phpTab, $aInputLines[0]));
		for($i = 1; $i < Count($aInputLines); $i++)
		{
			if(StrLen($aInputLines[$i]) > 0)
			{
				$aTemp = Explode(phpTab, $aInputLines[$i]);
				$this->m_aDictionary[] = $aTemp[$iColumn];
			}
		}
	}

	function RawWord($sWord)
	{
		$sWord = StrToLower($sWord);
		$sWord = StrReplace($sWord, '.', '');
		return $sWord;
	}

	function SortWord($sWord)
	{
		for($i = 0; $i < StrLen($sWord); $i++)
			$aChars[] = $sWord[$i];
		Sort($aChars);
		return Implode('', $aChars);
	}

	function Suggest($sWord)
	{
		$sResult = '';
		$sWord = $this->RawWord($sWord);
		$aList = array();

		foreach($this->m_aDictionary as $sDicWord)
		{
			$iDifference1 = Levenshtein($sWord, $sDicWord);
			$iDifference2 = Levenshtein($this->SortWord($sWord), $this->SortWord($sDicWord));

			if(($iDifference1 <= 2) or ($iDifference2 == 0))
				$aList[$sDicWord] = $iDifference1 * $iDifference2;
		}
		ASort($aList);

		$aResultList = array();	
		while(List($sKey, $iValue) = Each($aList))
			$aResultList[] = $sKey;

		return $aResultList;
	}

	function Check($sWord)
	{
		return In_Array($sWord, $this->m_aDictionary);
	}

}

/*
$cSpell = new CSpell('dictionary.txt', 'German');
foreach(Explode(' ', 'se scheint die schoene nonne') as $sWord)
{
	if($cSpell->Check($sWord))
		echo $sWord . ' ';
	else
		echo '<span class="Wrong">' . $sWord . '</span>' . ' (' . Implode(', ', $cSpell->Suggest($sWord)) . ') ';
}
*/

function TestClassSpell()
{
	$aTests[] = '$cSpell = new CSpell("dictionary.txt", "German"); return $cSpell->Check("Sonne") == TRUE;';
	$aTests[] = '$cSpell = new CSpell("dictionary.txt", "German"); return $cSpell->Check("Sun") == FALSE;';
	$aTests[] = '$cSpell = new CSpell("dictionary.txt", "German"); return $cSpell->Suggest("Wonne") == array("Sonne");';
	$aTests[] = '$cSpell = new CSpell("dictionary.txt", "German"); return $cSpell->Suggest("ABCDEFabcdef") == array();';

	return Test('ClassSpell', $aTests);
}

?>