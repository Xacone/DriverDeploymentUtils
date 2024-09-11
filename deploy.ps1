$date = Get-Date -Format "yyyyMMdd_HHmmss"

$newFileName = "KernelCallbacks_$date.sys"
$newServiceName = "KernelCallbacks_$date"

$command = "scp .\x64\Debug\KernelCallbacks.sys bob@172.20.226.13:C:\Users\Bob\Desktop\VSDrivers\$newFileName; ssh bob@172.20.226.13 `"sc.exe create $newServiceName binPath=C:\Users\Bob\Desktop\VSDrivers\$newFileName type=kernel && sc.exe start $newServiceName`""
Invoke-Expression $command