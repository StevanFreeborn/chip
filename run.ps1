Write-Host "[Compiling...]"

gcc main.c -o ./.bin/chip.exe -Wall -Wextra -lraylib -lopengl32 -lgdi32 -lwinmm

if ($LASTEXITCODE -ne 0) {
  Write-Host "[Failed to compiled]";
  exit 1;
}

Write-Host "[Compiled successfully]";

Write-Host "[Running..]"
Write-Host "`n"

./.bin/chip.exe $args[0]
