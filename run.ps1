Write-Host "[Compiling...]"

gcc main.c -o ./.bin/chip.exe

if ($LASTEXITCODE -ne 0) {
  Write-Host "[Failed to compiled]";
  exit 1;
}

Write-Host "[Compiled successfully]";

Write-Host "[Running..]"
Write-Host "`n"

./.bin/chip.exe $args[0]
