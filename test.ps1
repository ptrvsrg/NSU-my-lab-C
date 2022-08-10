if (Test-Path -Path "build_test")
{
    Remove-Item "build_test" -Force -Recurse 2>&1 > $null
}

if ($IsWindows)
{
    cmake -B build_test -DCMAKE_BUILD_TYPE=Release
    cmake --build build_test --config Release --target lab_test lab_src
    cmake --build build_test --target RUN_TESTS
}

if ($IsLinux)
{
    cmake -B build_test -DCMAKE_BUILD_TYPE=Release
    cmake --build build_test --target lab_test lab_src
    cmake --build build_test --target test
}

if (Test-Path ./Testing/Temporary/LastTestsFailed.log)
{
    exit 1
}