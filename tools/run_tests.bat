@echo off
echo ****************************************
echo * Batch building folder creator V. 1.4 *
echo ****************************************

REM if exist "%~dp0/Build" (
REM 	rmdir  /S /Q "%~dp0/Build"
REM )




cmake --build ../Build --config Debug -j4
cd ../Build
ctest -C Debug -VV

pause