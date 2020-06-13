@echo off
echo ****************************************
echo * Batch building folder creator V. 1.4 *
echo ****************************************

REM if exist "%~dp0/Build" (
REM 	rmdir  /S /Q "%~dp0/Build"
REM )


cd ../Build
ctest -C Debug

pause