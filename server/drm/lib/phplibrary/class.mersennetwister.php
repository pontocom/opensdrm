<?
///////////////////////////////////////////////////////////////////////////////
// Filename: class.mersennetwister.php
// Date: 23.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

class CMersenneTwister
{
	var $N, $M, $MATRIX_A, $UPPER_MASK, $LOWER_MASK, $mt, $mti, $mag01;

	function CMersenneTwister()
	{
		$this->N = 624;
		$this->M = 397;
		$this->MATRIX_A = '2567483615'; //0x9908b0df
		$this->UPPER_MASK = '2147483648'; //0x80000000
		$this->LOWER_MASK = '2147483647'; //0x7fffffff

		$this->mt = array();
		$this->mti = $this->N + 1;

		$this->mag01 = array('0', $this->MATRIX_A);
	}

	function init_genrand($s)
	{
		$this->mt[0] = MathAnd($s, '4294967295'); //0xffffffff
		for($this->mti = 1; $this->mti < $this->N; $this->mti++)
		{
			$d1 = MathShR($this->mt[$this->mti-1], '30');
			$d2 = MathXor($this->mt[$this->mti-1], $d1);
			$d3 = MathMul('1812433253', $d2);
			$d4 = MathAdd($this->mti, $d3);
			$d5 = MathAnd($d4, '4294967295'); //0xffffffff

			$this->mt[$this->mti] = $d5;
		}
	}

	function init_by_array($init_key)
	{
		$key_length = Count($init_key);

		$this->init_genrand('19650218');

		$i = 1;
		$j = 0;
		$k = $this->N > $key_length ? $this->N : $key_length;

		for(; $k; $k--)
		{
			$d1 = MathShR($this->mt[$i - 1], '30');
			$d2 = MathXor($this->mt[$i - 1], $d1);
			$d3 = MathMul('1664525', $d2);
			$d4 = MathXor($this->mt[$i], $d3);
			$d5 = MathAdd($init_key[$j], $d4);
			$d6 = MathAdd($j, $d5);
			$d7 = MathAnd($d6, '4294967295'); //0xffffffff

			$this->mt[$i] = $d7;

			$i++;
			$j++;

			if($i >= $this->N)
			{
				$this->mt[0] = $this->mt[$this->N-1];
				$i = 1;
			}

			if($j >= $key_length)
				$j = 0;
		}

		for($k = ($this->N - 1); $k; $k--)
		{
			$d1 = MathShR($this->mt[$i - 1], '30');
			$d2 = MathXor($this->mt[$i - 1], $d1);
			$d3 = MathMul('1566083941', $d2);
			$d4 = MathXor($this->mt[$i], $d3);
			$d5 = MathSub($d4, $i);
			$d6 = MathAnd($d5, '4294967295'); //0xffffffff

			$this->mt[$i] = $d6;

			$i++;

			if($i >= $this->N)
			{
				$this->mt[0] = $this->mt[$this->N - 1];
				$i = 1;
			}
		}

		$this->mt[0] = '2147483648'; // 0x80000000

	}

	function genrand_int32()
	{

		if($this->mti >= $this->N)
		{
			if($this->mti == $this->N + 1)
				$this->init_genrand(5489);
			
			for($kk = 0; $kk < ($this->N - $this->M); $kk++)
			{
				$d1 = MathAnd($this->mt[$kk], $this->UPPER_MASK);
				$d2 = MathAnd($this->mt[$kk + 1], $this->LOWER_MASK);

				$y = MathOr($d1, $d2);

				$d1 = MathShR($y, '1');
				$index = IntVal(MathAnd($y, '1'));
				$d2 = $this->mag01[$index];
				$d3 = MathXor($this->mt[$kk + $this->M], $d1);
				$d4 = MathXor($d3, $d2);

				$this->mt[$kk] = $d4;
			}

			for($kk = $kk; $kk < ($this->N - 1); $kk++)
			{
				$d1 = MathAnd($this->mt[$kk], $this->UPPER_MASK);
				$d2 = MathAnd($this->mt[$kk + 1], $this->LOWER_MASK);

				$y = MathOr($d1, $d2);

				$d1 = MathShR($y, '1');
				$index = IntVal(MathAnd($y, '1'));
				$d2 = $this->mag01[$index];
				$d3 = MathXor($this->mt[$kk + ($this->M - $this->N)], $d1);
				$d4 = MathXor($d3, $d2);

				$this->mt[$kk] = $d4;
			}

			$d1 = MathAnd($this->mt[$this->N - 1], $this->UPPER_MASK);
			$d2 = MathAnd($this->mt[0], $this->LOWER_MASK);

			$y = MathOr($d1, $d2);

			$d1 = MathShR($y, '1');
			$index = IntVal(MathAnd($y, '1'));
			$d2 = $this->mag01[$index];
			$d3 = MathXor($this->mt[$this->M - 1], $d1);
			$d4 = MathXor($d3, $d2);

			$this->mt[$this->N - 1] = $d4;

			$this->mti = 0;
		}

		$y = $this->mt[$this->mti++];

		$d1 = MathShR($y, '11');
		$y = MathXor($y, $d1);

		$d1 = MathShL($y, '7');
		$d2 = MathAnd($d1, '2636928640'); // 0x9d2c5680
		$y = MathXor($y, $d2);

		$d1 = MathShL($y, '15');
		$d2 = MathAnd($d1, '4022730752'); //0xefc60000
		$y = MathXor($y, $d2);

		$d1 = MathShR($y, '18');
		$y = MathXor($y, $d1);

		return $y;  
	}

}

/*
Init

0x123, 0x234, 0x345, 0x456

Results

1067595299  955945823  47729528  4107218783 4228976476 
3344332714 3355579695  227628506  810200273 2591290167 
2560260675 3242736208  646746669 1479517882 4245472273 
1143372638 3863670494 3221021970 1773610557 1138697238 
1421897700 1269916527 2859934041 1764463362 3874892047 
*/

function TestClassMersenneTwister()
{
	$aTests[] = '$c = New CMersenneTwister; $c->init_by_array(array(291, 564, 837, 1110)); return $c->genrand_int32() == 1067595299;';

	return Test('ClassMersenneTwister', $aTests);
}

?>