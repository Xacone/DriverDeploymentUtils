$drivers = driverquery /fo csv | ConvertFrom-Csv

$kernelDrivers = $drivers | Where-Object { $_."Nom du module" -like "KernelCa*" }

foreach ($driver in $kernelDrivers) {
    $driverName = $driver."Nom du module"
    Write-Output "Delete drv : $driverName"
    sc.exe delete $driverName
}
