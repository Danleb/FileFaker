$exe_name = $args[0]
TASKLIST /FI "IMAGENAME eq $exe_name" /FO TABLE /NH | Tee-Object -Variable cmdOutput
$CharArray = -split $cmdOutput 
$process_id = $CharArray[2]
Write-Output redirect-file
Write-Output $process_id
Write-Output C:\Users\TestUser\Desktop\1.txt
Write-Output C:\Users\TestUser\Desktop\2.txt
Write-Output ""