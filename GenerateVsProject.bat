@echo off
for /R %%f in (*.uproject) do (
    set UPROJECT="%%f"
)

if [%UPROJECT%]==[] (
    echo "ERROR: UE project file cannot be found!!"
    exit 1
)

@REM echo %UPROJECT%
"C:/Program Files/Epic Games/UE_4.27/Engine/Binaries/DotNET/UnrealBuildTool.exe"  -projectfiles -project=%UPROJECT% -game -rocket -progress