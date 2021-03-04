if ( -not (Test-Path -Path './out' -PathType Container))
{
	mkdir out
}
cd out
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release


