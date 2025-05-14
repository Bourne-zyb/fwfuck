@echo off
C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bin --bincombined --bincombined_padding=1,0xff --output "FS-F20\FS-F20.bin" "FS-F20\FS-F20.axf"
copy "FS-F20\FS-F20.bin"  "FS-F20.bin"
CalculateCRC 16 3976 "FS-F20.bin"

