@echo off
setlocal

echo ===========================================
echo ============== Build Assimp ===============
echo ===========================================
pushd extern\assimp

call cmake -S . -B . -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
    echo Error configuring Assimp with CMake
    popd
    exit /b 1
)

call cmake --build . --config Release
if errorlevel 1 (
    echo Error building Assimp Release
    popd
    exit /b 1
)

call cmake --build . --config Debug
if errorlevel 1 (
    echo Error building Assimp Debug
    popd
    exit /b 1
)

popd

echo ===========================================
echo ============ Build JoltPhysics ============
echo ===========================================
pushd extern\JoltPhysics\Build

call cmake_vs2022_cl.bat
if errorlevel 1 (
    echo Error running cmake_vs2022_cl.bat
    popd
    exit /b 1
)

pushd VS2022_CL

call cmake --build . --config Release
if errorlevel 1 (
    echo Error building JoltPhysics Release
    popd & popd
    exit /b 1
)

call cmake --build . --config Debug
if errorlevel 1 (
    echo Error building JoltPhysics Debug
    popd & popd
    exit /b 1
)

popd
popd

echo ===========================================
echo ===== Launch FishEngine.sln ====
echo ===========================================
start "./FishEngine.sln"
start "./secret.mp3"

endlocal
pause
