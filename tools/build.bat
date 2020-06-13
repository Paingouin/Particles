@echo off
echo ****************************************
echo * Batch building folder creator V. 1.4 *
echo ****************************************

REM cmake --build <build_directory> --target install --config <desired_config>

REM if exist "%~dp0/Build" (
REM 	rmdir  /S /Q "%~dp0/Build"
REM )

cd ..
mkdir Build
cd Build

cmake -H.. -B. -G"Visual Studio 15 2017 Win64"  -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_BUILD_TYPE=Debug 
)

pause