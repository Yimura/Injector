@echo off
DEL Injector.sln /Q
DEL Injector\Injector.vcxproj /Q
DEL Injector\Injector.vcxproj.filters /Q
DEL Injector\Injector.vcxproj.user /Q

premake5 vs2019
IF %ERRORLEVEL% NEQ 0 (
  PAUSE
)