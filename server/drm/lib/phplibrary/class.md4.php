<?
///////////////////////////////////////////////////////////////////////////////
// Filename: class.md4.php
// Date: 23.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

class CMD4
{
	var $A, $B, $C, $D;

	function F($X, $Y, $Z)
	{
		return MathOr(MathAnd($X, $Y), MathAnd(MathNot($X, 32), $Z));
	}

	function G($X, $Y, $Z)
	{
		return MathOr(MathOr(MathAnd($X, $Y), MathAnd($X, $Z)), MathAnd($Y, $Z));
	}

	function H($X, $Y, $Z)
	{
		return MathXor(MathXor($X, $Y), $Z);
	}

	function LShift($X, $S)
	{
		$X = MathAnd($X, '4294967295');
		return MathOr(MathAnd(MathShL($X, $S), '4294967295'), MathShR($X, MathSub(32, $S)));
	}

	function Round1(&$a, $b, $c, $d, $k, $s, $X)
	{
		$a = $this->LShift(MathAdd(MathAdd($a, $this->F($b, $c, $d)), $X[$k]), $s);
	}

	function Round2(&$a, $b, $c, $d, $k, $s, $X)
	{
		$a = $this->LShift(MathAdd(MathAdd(MathAdd($a, $this->G($b, $c, $d)), $X[$k]), '1518500249'), $s);
	}

	function Round3(&$a, $b, $c, $d, $k, $s, $X)
	{
		$a = $this->LShift(MathAdd(MathAdd(MathAdd($a, $this->H($b, $c, $d)), $X[$k]), '1859775393'), $s);
	}

	function Copy64(&$M, $in, $p = 0)
	{
		for($i = 0; $i < 16; $i++)
			$M[$i] = MathOr(MathOr(MathOr(MathShL($in[$i * 4 + 3 + $p], 24), MathShL($in[$i * 4 + 2 + $p], 16)), MathShL($in[$i * 4 + 1 + $p], 8)), MathShL($in[$i * 4 + 0 + $p], 0));
	}

	function Copy4(&$out, $x, $p = 0)
	{
		$out[0 + $p] = MathAnd($x, 255);
		$out[1 + $p] = MathAnd(MathShR($x, 8), 255);
		$out[2 + $p] = MathAnd(MathShR($x, 16), 255);
		$out[3 + $p] = MathAnd(MathShR($x, 24), 255);
	}

	function MDChunk(&$M)
	{
		for($i = 0; $i < 16; $i++)
			$X[$i] = $M[$i];

		$this->AA = $this->A;
		$this->BB = $this->B;
		$this->CC = $this->C;
		$this->DD = $this->D;

		$this->Round1($this->A, $this->B, $this->C, $this->D,   0,   3, $X);
		$this->Round1($this->D, $this->A, $this->B, $this->C,   1,   7, $X);
		$this->Round1($this->C, $this->D, $this->A, $this->B,   2,  11, $X);
		$this->Round1($this->B, $this->C, $this->D, $this->A,   3,  19, $X);
		$this->Round1($this->A, $this->B, $this->C, $this->D,   4,   3, $X);
		$this->Round1($this->D, $this->A, $this->B, $this->C,   5,   7, $X);
		$this->Round1($this->C, $this->D, $this->A, $this->B,   6,  11, $X);
		$this->Round1($this->B, $this->C, $this->D, $this->A,   7,  19, $X);
		$this->Round1($this->A, $this->B, $this->C, $this->D,   8,   3, $X);
		$this->Round1($this->D, $this->A, $this->B, $this->C,   9,   7, $X);
		$this->Round1($this->C, $this->D, $this->A, $this->B,  10,  11, $X);
		$this->Round1($this->B, $this->C, $this->D, $this->A,  11,  19, $X);
		$this->Round1($this->A, $this->B, $this->C, $this->D,  12,   3, $X);
		$this->Round1($this->D, $this->A, $this->B, $this->C,  13,   7, $X);
		$this->Round1($this->C, $this->D, $this->A, $this->B,  14,  11, $X);
		$this->Round1($this->B, $this->C, $this->D, $this->A,  15,  19, $X);

		$this->Round2($this->A, $this->B, $this->C, $this->D,   0,   3, $X);
		$this->Round2($this->D, $this->A, $this->B, $this->C,   4,   5, $X);
		$this->Round2($this->C, $this->D, $this->A, $this->B,   8,   9, $X);
		$this->Round2($this->B, $this->C, $this->D, $this->A,  12,  13, $X);
		$this->Round2($this->A, $this->B, $this->C, $this->D,   1,   3, $X);
		$this->Round2($this->D, $this->A, $this->B, $this->C,   5,   5, $X);
		$this->Round2($this->C, $this->D, $this->A, $this->B,   9,   9, $X);
		$this->Round2($this->B, $this->C, $this->D, $this->A,  13,  13, $X);
		$this->Round2($this->A, $this->B, $this->C, $this->D,   2,   3, $X);
		$this->Round2($this->D, $this->A, $this->B, $this->C,   6,   5, $X);
		$this->Round2($this->C, $this->D, $this->A, $this->B,  10,   9, $X);
		$this->Round2($this->B, $this->C, $this->D, $this->A,  14,  13, $X);
		$this->Round2($this->A, $this->B, $this->C, $this->D,   3,   3, $X);
		$this->Round2($this->D, $this->A, $this->B, $this->C,   7,   5, $X);
		$this->Round2($this->C, $this->D, $this->A, $this->B,  11,   9, $X);
		$this->Round2($this->B, $this->C, $this->D, $this->A,  15,  13, $X);

		$this->Round3($this->A, $this->B, $this->C, $this->D,   0,   3, $X);
		$this->Round3($this->D, $this->A, $this->B, $this->C,   8,   9, $X);
		$this->Round3($this->C, $this->D, $this->A, $this->B,   4,  11, $X);
		$this->Round3($this->B, $this->C, $this->D, $this->A,  12,  15, $X);
		$this->Round3($this->A, $this->B, $this->C, $this->D,   2,   3, $X);
		$this->Round3($this->D, $this->A, $this->B, $this->C,  10,   9, $X);
		$this->Round3($this->C, $this->D, $this->A, $this->B,   6,  11, $X);
		$this->Round3($this->B, $this->C, $this->D, $this->A,  14,  15, $X);
		$this->Round3($this->A, $this->B, $this->C, $this->D,   1,   3, $X);
		$this->Round3($this->D, $this->A, $this->B, $this->C,   9,   9, $X);
		$this->Round3($this->C, $this->D, $this->A, $this->B,   5,  11, $X);
		$this->Round3($this->B, $this->C, $this->D, $this->A,  13,  15, $X);
		$this->Round3($this->A, $this->B, $this->C, $this->D,   3,   3, $X);
		$this->Round3($this->D, $this->A, $this->B, $this->C,  11,   9, $X);
		$this->Round3($this->C, $this->D, $this->A, $this->B,   7,  11, $X);
		$this->Round3($this->B, $this->C, $this->D, $this->A,  15,  15, $X);

		$this->A += $this->AA;
		$this->B += $this->BB;
		$this->C += $this->CC;
		$this->D += $this->DD;

		$this->A = MathAnd($this->A, '4294967295'); //0xFFFFFFFF
		$this->B = MathAnd($this->B, '4294967295'); //0xFFFFFFFF
		$this->C = MathAnd($this->C, '4294967295'); //0xFFFFFFFF
		$this->D = MathAnd($this->D, '4294967295'); //0xFFFFFFFF
	}

	function MD4Transform($sString)
	{
		$in = array();
		for($i = 0; $i < StrLen($sString); $i++)
			$in[$i] = Ord($sString[$i]);

		$n = Count($in);

		$b = $n * 8;
		$M = array();
		$out = array();

		$this->A = 1732584193;
		$this->B = 4023233417;
		$this->C = 2562383102;
		$this->D = 271733878;

		$p = 0;
		while($n > 64)
		{
			Copy64($M, $in, $p);
			MDChunk($M);
			$p += 64;
			$n -= 64;
		}

		for($i = 0; $i < 128; $i++)
			$buf[$i] = 0;

		for($i = 0; $i < $n; $i++)
			$buf[$i] = $in[$i];
		$buf[$n] = 128;

		if($n <= 55)
		{
			$this->Copy4($buf, $b, 56);
			$this->Copy64($M, $buf);
			$this->MDChunk($M);
		}
		else
		{
			$this->Copy4($buf, $b, 120);
			$this->Copy64($M, $buf);
			$this->MDChunk($M);
			$this->Copy64($M, $buf, 64);
			$this->MDChunk($M);
		}

		/*
		for($i = 0; $i < 128; $i++)
			$buf[$i] = 0;
		Copy64($M, $buf);
		*/

		$this->Copy4($out, $this->A);
		$this->Copy4($out, $this->B, 4);
		$this->Copy4($out, $this->C, 8);
		$this->Copy4($out, $this->D, 12);

		$sout = '';
		for($i = 0; $i < 16; $i++)
			$sout .= Chr($out[$i]);
		return $sout;
	}
}

function MD4($sString)
{
	$cMD4 = new CMD4;
	return $cMD4->MD4Transform($sString);
}

function MD4Hex($sString)
{
	return StrPadLeft(MathStrToHex(MD4($sString)), 32, '0');
}

function TestClassMD4()
{
	$aTests[] = 'return MD4Hex("") == "31d6cfe0d16ae931b73c59d7e0c089c0";';
	$aTests[] = 'return MD4Hex("a") == "bde52cb31de33e46245e05fbdbd6fb24";';
	$aTests[] = 'return MD4Hex("abc") == "a448017aaf21d8525fc10ae87aa6729d";';
	$aTests[] = 'return MD4Hex("message digest") == "d9130a8164549fe818874806e1c7014b";';
	$aTests[] = 'return MD4Hex("abcdefghijklmnopqrstuvwxyz") == "d79e1c308aa5bbcdeea8ed63df412da9";';
	$aTests[] = 'return MD4Hex("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") == "043f8582f241db351ce627e153e7f0e4";';

	return Test('ClassMD4', $aTests);
}

?>