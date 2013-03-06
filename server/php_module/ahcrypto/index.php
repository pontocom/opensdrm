<?
	//dl("php_ahcrypto.dll");
	
	$a = ahrsa_create_keys(1024);
	if ($a) {
		foreach ($a as $chv => $val) {
			print($chv."=>".bin2hex($val)); print("; ");
		}
	} else {
		print("moscas");
	}
	print("<br>");
	
	//$t_hex = "4142434445";
	//$t_hex_len = strlen($t_hex);
	//$t = pack('H'.$t_hex_len, $t_hex);
	//print($t."<br>");
	//return;

	if ($a) {
		$n = $a["n"]; print("n=".bin2hex($n)."<br>");
		$e = $a["e"]; print("e=".bin2hex($e)."<br>");
		$d = $a["d"]; print("d=".bin2hex($d)."<br>");
		$p = $a["p"]; print("p=".bin2hex($p)."<br>");
		$q = $a["q"]; print("q=".bin2hex($q)."<br>");
	} else {
		$n_hex = "c2d94eae1db69466ef0efa06e11d2840e3706024d2ecdd82f25d2aa2e4fc0636f71bc7f51d80db53cb6e125abce03d1509c0107f22c142dbef0c17d520a5f20ff22dfba3e0df382d4676c2d100d1baa0c050d125b3004a33bbb391831126585e3acc0c3d491bf68d74c1e07e6e93df68f93c2b399fd9c83466824c4f465cb049";
		$e_hex = "11";
		$d_hex = "013a60c3c9548316294e13dc8c2cd28a16f22fef05a1c3027051426b83982d37341cade61dc409fcc36ef221ea9a271b8fa878931644fb94bf5af78b8fcd6a66463d43ce4be3c2684d42c9084400f5559d8e11331ff51945db4df05776c276f0ac486dccf1406aac662aed5723f2dbf203d4711bb32c2dafb3b9847d6f2abbf5";
		$p_hex = "e5fe229ef2a9f03321e7e027e39b8ae253fa9f10c14d32bd36cecbf5270ad3c6629ec71f090f94d0006abe0ebbd4d9b168ff76df2fdfb47e57a6422aee82f0a5";
		$q_hex = "d8e1d27de50b74aa41d4f4344c9e2158c59e33ba7874c2d5434124ada43f2217bb49376e0eca4438cc66b318158eb3b7db3bef2f634318255535ec661c07ebd5";

		$n_hex_len = strlen($n_hex);
		$e_hex_len = strlen($e_hex);
		$d_hex_len = strlen($d_hex);
		$p_hex_len = strlen($p_hex);
		$q_hex_len = strlen($q_hex);
	
		$n = pack('H'.$n_hex_len, $n_hex); print("n=".bin2hex($n)."<br>");
		$e = pack('H'.$e_hex_len, $e_hex); print("e=".bin2hex($e)."<br>");
		$d = pack('H'.$d_hex_len, $d_hex); print("d=".bin2hex($d)."<br>");
		$p = pack('H'.$p_hex_len, $p_hex); print("p=".bin2hex($p)."<br>");
		$q = pack('H'.$q_hex_len, $q_hex); print("q=".bin2hex($q)."<br>");
	}
	print("<br>");

	$b = ahrsa_cipher("1234567890", $n, $e);
	print("cblk=".bin2hex($b)."<br>");

	$c = ahrsa_decipher($b, $n, $e, $d, $p, $q);
	print("data=".($c ? $c : " --- FALHA --- ")."<br>");

	print("<br>");

	$sig = ahrsa_sign("ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz", $n, $e, $d, $p, $q);
	print("Signature=".bin2hex($sig)."<br>"."Length=".strlen($sig)."<br>");

	$old_8 = $sig[8];
	$sig[8] = 'A';
	print("Signature=".bin2hex($sig)."<br>");
	print("Signature:".(ahrsa_verify("ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz", $sig, $n, $e) ? "OK" : "NOK")."<br>");

	$sig[8] = $old_8;
	print("Signature=".bin2hex($sig)."<br>");
	print("Signature:".(ahrsa_verify("ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz", $sig, $n, $e) ? "OK" : "NOK")."<br>");

	print("<br><br><br>");

	//$k_hex = "A0000000A0000000A0000000A0000000";
	//_hex = "849325AB83245FDE98896BDA78565FEDEE5343457457AAC0";
	$k_hex = "7893333654326BAFCDEF555576B00009032973234FFAD289373285AB986786ED";
	$k_hex_len = strlen($k_hex);
	$k = pack('H'.$k_hex_len, $k_hex);

	//$texto_a_cifrar = "Texto de teste para a implementação de aes no módulo php ahcrypto. Este texto está protegido pelas mais restritivas leis de direitos de autor. É proíbida a sua cópia ou divulgação.";
	$texto_a_cifrar = "XXXXXXYYYYYYZZZZZZWWWWWWKKKKKKMMMMMMNNNNNNPPPPPPXXXXXXYYYYYYZZZZZZWWWWWWKKKKKKMMMMMMNNNNNNPPPPPP";
	$texto_cifrado = ahaes_cipher($texto_a_cifrar, $k);
	if ($texto_cifrado) {
		print("Criptograma: ".bin2hex($texto_cifrado));
		$texto_recuperado = ahaes_decipher($texto_cifrado, $k);
	} else {
		$texto_recuperado = " --- --- --- --- ";
	}

	print("<p>".$texto_recuperado."</p>");
	
	print("PHP sucks.");
	phpinfo();
?>

