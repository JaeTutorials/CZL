class a
{
	var v1=1;
	var v2=2;
	var v3="foobar";
}

main
{
	f = fopen("datas.txt", "3wb+")
	
	a = []
	for i in (0, 1000000-1)
		a[i] = new a

	fwrite(f, a)
	echo(clock())

	a = fread(f)
	echo(clock())
}
