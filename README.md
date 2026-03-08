# FileFaker

FileFaker is a project showing how to hook file I/O functions using the DLL injection and re-writing function addresses in .idata section of loaded PE modules of process.

## How to use

You can use FileFaker as a command line util or as a library.

To use from cmd:

Here, the first line is command, the second line is the process ID that you want to inject into. Then write path to the file that you want to be redirected, and then path to file you want to open instead of previous:

```cmd
redirect-file
11540
C:\Users\TestUser\Desktop\TrueData.txt
C:\Users\TestUser\Desktop\FakeData.txt
```

You can get the process ID like this:

```cmd
TASKLIST /FI "IMAGENAME eq notepad++.exe" /FO TABLE /NH
```

After successful execution, you will get the info message with the redirection ID. Now when you open file TrueData.txt, you get contents of the file FakeData.txt.

You can use poweshell script RedirectTemplate.ps1 to look for process ID by its exe name.

```powershell
.\RedirectTemplate.ps1 "notepad.exe"
```

Next commands are supported:

```console
redirect-file|rf <process_id> <file_path_from> <file_path_to>
redirect-files|rfs <process_id> <file_path_to>
remove|rm <redirection_id>
```

## How to build

You need the next tools:

* [CMake](https://cmake.org/download/)
* [Python](https://www.python.org/downloads/)
* [Conan package manager](https://conan.io/downloads) (major version 2)
* [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/)

Build is done using CMake. [Conan](https://conan.io/) is used as a package manager, dependencies are downloaded and installed automatically on build.

To build from cmd:

```cmd
mkdir build
cmake -S ./ -B ./build -DCMAKE_BUILD_TYPE=Release
cmake --build ./build
```

## Current state, advantages and disadvantages

At the current moment, FileFaker is available for Windows x64 platform.

File functions that are currently hooked:

- fopen
- fopen_s
- \_fsopen
- \_wfsopen
- \_wfopen
- \_wfopen_s
- freopen
- \_wfreopen
- CreateFileA
- CreateFileW

As function CreateFile is used for pipes creation, this tool also can hook pipes creation.

## How it works

The app consists of two parts: command line executable - server, and loadable library - client. When you specify the process ID, server loads the client library into that process. Client hooks file functions in executable and all loaded libraries.

Communication between server and client is implemented using pipes.

Tests are implemented using GTest.

## Project structure

## License and using

This project is licensed without any guarantees under the MIT license.

If you have any questions, please contact me mstr.danila@gmail.com
