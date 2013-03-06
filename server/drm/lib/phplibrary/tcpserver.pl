use	IO::Socket;

$hSocket = new	IO::Socket::INET(LocalPort =>	'6680',	Reuse	=> 1,	Listen =>	5);

while	($hClient = $hSocket->accept())
{
	if (fork() ==	0)
	{
		while	(<$hClient>)
		{
			chomp;
			print	$_, "\n";
		}
		exit 0;
	}
}