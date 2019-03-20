@echo off 
if not exist bin ( mkdir bin )
pushd bin
    cmake -G "Visual Studio 15 2017" ..
popd


 