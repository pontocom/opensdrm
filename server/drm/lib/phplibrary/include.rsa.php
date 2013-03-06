<?
///////////////////////////////////////////////////////////////////////////////
// Filename: include.rsa.php
// Date: 23.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

/*

RSA (Rivest, Shamir, Adleman) Public Key Encryption

p := Primzahl 1
q := Primzahl 2

q > p

n := p * q

phi := (p - 1) * (q - 1)

gcd(e, phi) = 1 => e

t := phi / gcd(p - 1, q - 1)
(e * d) mod t = 1 => s


m := Klartext
c := Chiffretext
s := Signatur

RSA Public Key

n := Modulus
e := Public Exponent

RSA Private Key

n := Modulus
d := Private Exponent



2048 Bit = 256 Byte = 3.231701e616
1024 Bit = 128 Byte = 1.797693e308
512 Bit  =  64 Byte = 1.340781e154

*/

//Encryption (Public Encryption)
function RSAE($n, $e, $m)
{
	// 0 < m < (n - 1)

	$c = MathPowMod($m, $e, $n);
	return $c;
}

//Decryption (Private Decryption)
function RSAD($n, $d, $c)
{
  //c = m ** e % n;
  // 0 < c < (n - 1)

	$m = MathPowMod($c, $d, $n);
	return $m;
}

//Signature (Private Encryption)
function RSAS($n, $d, $m)
{
	// m = c ** d % n;
	// 0 < m < (n - 1)

	$s = MathPowMod($m, $d, $n);
	return $s;
}

//Verification (Public Decryption)
function RSAV($n, $e, $s)
{
	// 0 < s < (n - 1)

	$m = MathPowMod($s, $e, $n);
	return $m;
}

/*

function RSAES_OAEP_ENCRYPT($n, $e, $M, $P)
{
	//EM = EME-OAEP-ENCODE(M, P, k-1);
	//m = OS2I(EM);
	//c = RSAE(n, e, m);
	//C = I2OS(c, k);
	//return C;
}

function RSAES_OAEP_DECRYPT($n, $d, $C, $P)
{
  //c = OS2I(C);
	//m = RSAD(n, d, c);
	//EM = I2OS(m, k-1);
	//M = EME_OAEP_DECODE(EM, P);
  //return M
}

function RSASSA_PSS_SIGN($n, $d, $M)
{
	//EM = EMSA_PSS_ENCODE(M, modBits-1)
	//m = OS2I(EM)
	//s = RSAS(n, d, m)
	//S = I2OS(s, k)
	//return S
}

function RSASSA_PSS_VERIFY($n, $e, $M, $S)
{
	//s = OS2I(S)
	//m = RSAV(n, e, s)
	//EM = I2OSP(m, emLen)
	//return EMSA_PSS_VERIFY(M, EM, modBits-1)
}

function EME_OAEP_ENCODE($M, $P, $emLen)
{
}

function EME_OAEP_DECODE($EM, $P)
{
}

function EMSA_PSS_ENCODE($M, $emBits)
{
}

function EMSA_PSS_VERIFY($M, $EM, $emBits)
{
}

//Mask Generation Function
function MGF($Z, $l)
{
  $T = '';
	$hLen = 32; //Len(MD5)
	for($i = 0; $i < ($l / $hLen); $i++)
	{
		$C = I2OS($i, 4);
		$T .= MD5($Z . $C);
	}
	return SubStr($T, 0, $l);
}

*/

function RSAKeys($p, $q, &$n, &$e, &$d)
{
	$n = MathMul($p, $q);
	$phi = MathMul(MathSub($p, 1), MathSub($q, 1));

	$e = '3';

	while(MathGCD($e, $phi) != '1')
		$e = MathAdd($e, '1');

	$t = MathDiv($phi, MathGCD(MathSub($p, 1), MathSub($q, 1)));
	$d = MathInv($e, $t);
}

function TestIncludeRSA()
{
	$aTests[] = 'RSAKeys("976109", "991987", $n, $e, $d); return $n == "968287438583";';
	$aTests[] = 'RSAKeys("976109", "991987", $n, $e, $d); return $e == "5";';
	$aTests[] = 'RSAKeys("976109", "991987", $n, $e, $d); return $d == "96828547049";';
	$aTests[] = '$c = RSAE("968287438583", "5", "123456789"); return $c == "847665801936";';
	$aTests[] = '$m = RSAD("968287438583", "96828547049", "847665801936"); return $m == "123456789";';

	return Test('IncludeRSA', $aTests);
}

?>