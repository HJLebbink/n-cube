
$files = Get-ChildItem -Path .\ -Filter *.dot

#Write-Host $files

for ($i=0; $i -lt $files.Count; $i++) {
#	$Command = '"C:\Program Files (x86)\Graphviz2.38\bin\dot.exe" ' + $files[$i].BaseName + '.dot -Tpdf -o ' + $files[$i].FullName + '.pdf'
	$Command = "'C:\Program Files (x86)\Graphviz2.38\bin\dot.exe' " + $files[$i].BaseName + '.dot -Tpdf -O'
	Write-Host $Command
#	write-host $files[$i].FullName
	Invoke-Expression '$Command'
}
