@echo off 
rem build solution only with release type, using cmake could build both debug and release solution.
if not exist bin ( mkdir bin )
set build_type=Release
pushd bin
    cmake -G "Visual Studio 15 2017" .. -DCMAKE_BUILD_TYPE=%build_type%

	cmake --build . --config %build_type%
popd
echo please check binaries at:"deps\NPLRuntime\ParaWorld\bin32\cef3"



 