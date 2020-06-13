@echo off
echo ****************************************
echo * Batch building folder creator V. 1.4 *
echo ****************************************

REM if exist "%~dp0/Build" (
REM 	rmdir  /S /Q "%~dp0/Build"
REM )

cd ..
mkdir Build_release
cd Build_release

cmake -H.. -B. -G"Visual Studio 15 2017 Win64"  -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_BUILD_TYPE=Release 
)

rem install phase
cmake --build . --target install --config Release
)

rem package phase
cmake --build . --target package --config Release
)


rem package phase
REM cmake --build . --target Package --config Debug
rem )


pause