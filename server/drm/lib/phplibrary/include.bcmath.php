<?
///////////////////////////////////////////////////////////////////////////////
// Filename: include.bcmath.php
// Date: 23.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

//Octet String To Integer
function MyOS2I($sString)
{
	$qNumber = '0';

	$iStrLen = StrLen($sString);
	for($i = 0; $i < $iStrLen; $i++)
	{
		$iChar = Ord($sString[$iStrLen - $i - 1]);
		$qNumber = bcadd($qNumber, bcmul($iChar, bcpow('256', $i)));
	}

	return $qNumber;
}

//Integer To Octet String
function MyI2OS($qNumber, $iLength = -1)
{
	$sString = '';

	if($iLength < 0)
	{
		while($qNumber != '0')
		{
			$sString = Chr(bcmod($qNumber, '256')) . $sString;
			$qNumber = bcdiv($qNumber, '256');
		}
	}
	else
	{
		for($i = 0; $i < $iLength; $i++)
		{
			$sString = Chr(bcmod($qNumber, '256')) . $sString;
			$qNumber = bcdiv($qNumber, '256');
		}
	}

	return $sString;
}

/*
7 ^ 23 = 27368747340080916343
27368747340080916343 mod 311 = 234
*/
function BCPowMod($qBase, $qExponent, $qModulus)
{
	if($qModulus == '0')
		return BCPow($qBase, $qExponent);

	$qBase = BCMod($qBase, $qModulus);

	$bin_e = StrRev(MathDecToBin($qExponent));
	$qResult = '1';

	$ae[0] = $qBase;
	for($i = 1; $i < StrLen($bin_e); $i++)
		$ae[$i] = BCMod(BCPow($ae[$i - 1], 2), $qModulus);

	for($i = 0; $i < StrLen($bin_e); $i++)
		if($bin_e[$i] == '1')
		  $qResult = BCMod(BCMul($qResult, $ae[$i]), $qModulus);

	return $qResult;
}

/*
Euklids Alogirthmus um folgende Gleichung a * x mod n = 1
nach x aufzuloesen, mit der Bedingung, dass 0 < a < n
*/
function BCInv($a, $n)
{
	$x = '';

	$g[0] = $n;
	$g[1] = $a;
	$v[0] = '0';
	$v[1] = '1';

	$i = 1;
	while($g[$i] != '0')
	{
		$g[$i + 1] = BCMod($g[$i - 1], $g[$i]);
		$y = BCDiv($g[$i - 1], $g[$i]);
		$t = BCMul($y, $v[$i]);
		$v[$i + 1] = $v[$i - 1];
		$v[$i + 1] = BCSub($v[$i + 1], $t);
		$i++;
	}

	$x = $v[$i - 1];
	if(bccomp($x, '0') < 0)
		$x = BCAdd($x, $n);

	return $x;
}

/*
Schon wieder der nette Euklid der uns folgenden Alogorithmus
zur Berechnung des GGT mit auf den Weg gegeben hat
*/
function BCGCD($qOperand1, $qOperand2)
{
	if($qOperand1 == '0' Or $qOperand2 == '0')
		return '0';

	do
	{
		$qModulo = BCMod($qOperand1, $qOperand2);
		$qOperand1 = $qOperand2;
		$qOperand2 = $qModulo;
	}
	while($qModulo != '0');
	return $qOperand1;
}

function BCAnd($qOperand1, $qOperand2)
{
	$qOperand1 = MyI2OS($qOperand1);
	$qOperand2 = MyI2OS($qOperand2);
	StrEqualize($qOperand1, $qOperand2, phpNul);

	$qResult = '';
	for($i = 0; $i < StrLen($qOperand1); $i++)
	{
		$iValue1 = Ord($qOperand1[$i]);
		$iValue2 = Ord($qOperand2[$i]);
		$qResult .= Chr($iValue1 & $iValue2);
	}
	return MyOS2I($qResult);
}

function BCOr($qOperand1, $qOperand2)
{
	$qOperand1 = MyI2OS($qOperand1);
	$qOperand2 = MyI2OS($qOperand2);
	StrEqualize($qOperand1, $qOperand2, phpNul);

	$qResult = '';
	for($i = 0; $i < StrLen($qOperand1); $i++)
	{
		$iValue1 = Ord($qOperand1[$i]);
		$iValue2 = Ord($qOperand2[$i]);
		$qResult .= Chr($iValue1 | $iValue2);
	}
	return MyOS2I($qResult);
}

function BCXor($qOperand1, $qOperand2)
{
	$qOperand1 = MyI2OS($qOperand1);
	$qOperand2 = MyI2OS($qOperand2);
	StrEqualize($qOperand1, $qOperand2, phpNul);

	$qResult = '';
	for($i = 0; $i < StrLen($qOperand1); $i++)
	{
		$iValue1 = Ord($qOperand1[$i]);
		$iValue2 = Ord($qOperand2[$i]);
		$qResult .= Chr($iValue1 ^ $iValue2);
	}
	return MyOS2I($qResult);
}

function BCNot($qOperand, $iBits)
{
	$qOperand = MathDecToBin($qOperand);
	$qOperand = StrPadLeft($qOperand, $iBits, '0');

	$qResult = '';
	for($i = 0; $i < StrLen($qOperand); $i++)
		$qResult .= ($qOperand[$i] == '1') ? '0' : '1';
	return MathAnyToDec(StrRight($qResult, $iBits), 2);
}

function TestIncludeBCMath()
{
	$aTests[] = 'return BCAnd(980299, 289273) == 287049;';
	$aTests[] = 'return BCAnd(980299, 0) == 0;';
	$aTests[] = 'return BCOr(980299, 289273) == 982523;';
	$aTests[] = 'return BCOr(980299, 0) == 980299;';
	$aTests[] = 'return BCXor(980299, 289273) == 695474;';
	$aTests[] = 'return BCNot(980299, 20) == 68276;';
	$aTests[] = 'return BCNot(5, 1) == 0;';
	$aTests[] = 'return BCNot(5, 2) == 2;';
	$aTests[] = 'return BCNot(5, 3) == 2;';
	$aTests[] = 'return BCNot(5, 6) == 58;';
	$aTests[] = 'return BCNot(5, 20) == 1048570;';
	$aTests[] = 'return BCGCD(123456789, 987654321) == 9;';
	$aTests[] = 'return BCGCD(123456789, 0) == 0;';
	$aTests[] = 'return BCGCD(0, 0) == 0;';
	$aTests[] = 'return BCGCD(123456789, 1) == 1;';
	$aTests[] = 'return BCGCD(5, 3) == 1;';
	$aTests[] = 'return BCPowMod(123, 456, 789) == 699;';
	$aTests[] = 'return BCPowMod(3, 5, 7) == 5;';
	$aTests[] = 'return BCPowMod(3, 0, 7) == 1;';
	$aTests[] = 'return BCPowMod(0, 5, 7) == 0;';
	$aTests[] = 'return BCPowMod(3, 5, 0) == 243;';
	$aTests[] = 'return BCInv(3, 5) == 2;';
	$aTests[] = 'return BCInv(123456, 789132) == 784012;';

	return Test('IncludeBCMath', $aTests);
}

?>