# wshminer

[![standard-readme compliant](https://img.shields.io/badge/readme%20style-standard-brightgreen.svg)](https://github.com/RichardLitt/standard-readme)
[![Gitter](https://img.shields.io/gitter/room/nwjs/nw.js.svg)][Gitter]
[![Releases](https://img.shields.io/github/downloads/wiseplat/wshminer/total.svg)][Releases]

> Wiseplat miner with OpenCL, CUDA and stratum support

The wshminer is an Wiseplat GPU mining worker. This is the actively maintained version of wshminer. It originates from [cpp-wiseplat] project (where GPU mining has been discontinued) and builds on the improvements made in [Genoil's fork]. See [FAQ](#faq) for more details.

### Features

- OpenCL mining
- Nvidia CUDA mining
- realistic benchmarking against arbitrary epoch/DAG/blocknumber
- on-GPU DAG generation (no more DAG files on disk)
- stratum mining without proxy
- OpenCL devices picking
- farm failover (getwork + stratum)


## Table of Contents

- [Install](#install)
- [Usage](#usage)
- [Build](#build)
  - [Continuous Integration and development builds](#continuous-integration-and-development-builds)
  - [Building from source](#building-from-source)
  - [CMake configuration options](#cmake-configuration-options)
- [Maintainer](#maintainer)
- [Contribute](#contribute)
- [F.A.Q.](#faq)


## Install

[![Releases](https://img.shields.io/github/downloads/wiseplat/wshminer/total.svg)][Releases]

Standalone **executables** for _Linux_, _macOS_ and _Windows_ are provided in
the [Releases] section.
Download an archive for your operating system and unpack the content to a place
accessible from command line. The wshminer is ready to go.

| Builds | Release | Date |
| ------ | ------- | ---- |
| Last    | [![GitHub release](https://img.shields.io/github/release/wiseplat/wshminer/all.svg)](https://github.com/wiseplat/wshminer/releases) | [![GitHub Release Date](https://img.shields.io/github/release-date-pre/wiseplat/wshminer.svg)](https://github.com/wiseplat/wshminer/releases) |
| Stable  | [![GitHub release](https://img.shields.io/github/release/wiseplat/wshminer.svg)](https://github.com/wiseplat/wshminer/releases) | [![GitHub Release Date](https://img.shields.io/github/release-date/wiseplat/wshminer.svg)](https://github.com/wiseplat/wshminer/releases) |


## Usage

The **wshminer** is a command line program. This means you launch it either
from a Windows command prompt or Linux console, or create shortcuts to
predefined command lines using a Linux Bash script or Windows batch/cmd file.
For a full list of available command, please run

```sh
wshminer --help
```


## Build

### Continuous Integration and development builds

| CI            | OS            | Status  | Development builds |
| ------------- | ------------- | -----   | -----------------  |
| [Travis CI]   | Linux, macOS  | [![Travis CI](https://img.shields.io/travis/wiseplat/wshminer.svg)][Travis CI]    | ✗ No build artifacts, [Amazon S3 is needed] for this |
| [AppVeyor]    | Windows       | [![AppVeyor](https://img.shields.io/appveyor/ci/wiseplat/wshminer.svg)][AppVeyor] | ✓ Build artifacts available for all PRs and branches |

The AppVeyor system automatically builds a Windows .exe for every commit. The latest version is always available [on the landing page](https://ci.appveyor.com/project/wiseplat/wshminer) or you can [browse the history](https://ci.appveyor.com/project/wiseplat/wshminer/history) to access previous builds.

To download the .exe on a build under 'JOB NAME' select 'Configuration: Release', choose 'ARTIFACTS' then download the zip file.


### Building from source

This project uses [CMake] and [Hunter] package manager.

1. Make sure git submodules are up to date

   ```sh
   git submodule update --init --recursive
   ```

2. Create a build directory.

   ```sh
   mkdir build; cd build
   ```

3. Configure the project with CMake. Check out additional
   [configuration options](#cmake-configuration-options).

   ```sh
   cmake ..
   ```

   Note: In Windows, it is possible to have issues with VS 2017 compilers, in that case, use VS 2017 installer to get VS 2015 compilers and use:

   ```sh
   cmake .. -G "Visual Studio 15 2017 Win64" -Tv140
   ```

4. Build the project using [CMake Build Tool Mode]. This is a portable variant
   of `make`.

   ```sh
   cmake --build .
   ```

   Note: In Windows, it is possible to have compiler issues if you don't specify build config. In that case use:

   ```sh
   cmake --build . --config Release
   ```

5. _(Optional, Linux only)_ Install the built executable.

   ```sh
   sudo make install
   ```

#### OpenCL support on Linux

If you're planning to use [OpenCL on Linux](https://github.com/ruslo/hunter/wiki/pkg.opencl#pitfalls)
you have to install OpenGL libraries. E.g. on Ubuntu run:

```sh
sudo apt-get install mesa-common-dev
```

#### Disable Hunter

If you want to install dependencies yourself or use system package manager
you can disable Hunter by adding
[-DHUNTER_ENABLED=OFF](https://docs.hunter.sh/en/latest/reference/user-variables.html#hunter-enabled)
to configuration options.

### CMake configuration options

Pass these options to CMake configuration command, e.g.

```sh
cmake .. -DWSHASHCUDA=ON -DWSHASHCL=OFF
```

- `-DWSHASHCL=ON` - enable OpenCL mining, `ON` by default,
- `-DWSHASHCUDA=ON` - enable CUDA mining, `OFF` by default.


## Maintainer

[![Gitter](https://img.shields.io/gitter/room/wiseplat/wshminer.svg)][Gitter]

- Paweł Bylica [@chfast](https://github.com/chfast)


## Contribute

[![Gitter](https://img.shields.io/gitter/room/wiseplat/wshminer.svg)][Gitter]

To meet the community, ask general questions and chat about wshminer join [the wshminer channel on Gitter][Gitter].

All bug reports, pull requests and code reviews are very much welcome.


## License

Licensed under the [GNU General Public License, Version 3](LICENSE).


## F.A.Q

1. Why is my hashrate with Nvidia cards on Windows 10 so low?

   The new WDDM 2.x driver on Windows 10 uses a different way of addressing the GPU. This is good for a lot of things, but not for WSH mining.
   For Kepler GPUs: I actually don't know. Please let me know what works best for good old Kepler.
   For Maxwell 1 GPUs: Unfortunately the issue is a bit more serious on the GTX750Ti, already causing suboptimal performance on Win7 and Linux. Apparently about 4MH/s can still be reached on Linux, which, depending on WSH price, could still be profitable, considering the relatively low power draw.
   For Maxwell 2 GPUs: There is a way of mining WSH at Win7/8/Linux speeds on Win10, by downgrading the GPU driver to a Win7 one (350.12 recommended) and using a build that was created using CUDA 6.5.
   For Pascal GPUs: You have to use the latest WDDM 2.1 compatible drivers in combination with Windows 10 Anniversary edition in order to get the full potential of your Pascal GPU.

2. Why is a GTX 1080 slower than a GTX 1070?

   Because of the GDDR5X memory, which can't be fully utilized for WSH mining (yet).

3. Are AMD cards also affected by slowdowns with increasing DAG size?

   Only GCN 1.0 GPUs (78x0, 79x0, 270, 280), but in a different way. You'll see that on each new epoch (30K blocks), the hashrate will go down a little bit.

4. Can I still mine WSH with my 2GB GPU?

   Not really, your VRAM must be above the DAG size (Currently about 2.15 GB.) to get best performance. Without it severe hash loss will occur.

5. What are the optimal launch parameters?

   The default parameters are fine in most scenario's (CUDA). For OpenCL it varies a bit more. Just play around with the numbers and use powers of 2. GPU's like powers of 2.

6. What does the `--cuda-parallel-hash` flag do?

   [@davilizh](https://github.com/davilizh) made improvements to the CUDA kernel hashing process and added this flag to allow changing the number of tasks it runs in parallel. These improvements were optimised for GTX 1060 GPUs which saw a large increase in hashrate, GTX 1070 and GTX 1080/Ti GPUs saw some, but less, improvement. The default value is 4 (which does not need to be set with the flag) and in most cases this will provide the best performance.

7. What is wshminer's relationship with [Genoil's fork]?

   [Genoil's fork] was the original source of this version, but as Genoil is no longer consistently maintaining that fork it became almost impossible for developers to get new code merged there. In the interests of progressing development without waiting for reviews this fork should be considered the active one and Genoil's as legacy code.

8. Can I CPU Mine?

   No, use gwsh, the go program made for wiseplat by wiseplat.



[Amazon S3 is needed]: https://docs.travis-ci.com/user/uploading-artifacts/
[AppVeyor]: https://ci.appveyor.com/project/wiseplat/wshminer
[CMake]: https://cmake.org/
[CMake Build Tool Mode]: https://cmake.org/cmake/help/latest/manual/cmake.1.html#build-tool-mode
[cpp-wiseplat]: https://github.com/wiseplat/cpp-wiseplat
[Genoil's fork]: https://github.com/Genoil/cpp-wiseplat
[Gitter]: https://gitter.im/wiseplat/wshminer
[Hunter]: https://docs.hunter.sh/
[Releases]: https://github.com/wiseplat/wshminer/releases
[Travis CI]: https://travis-ci.org/wiseplat/wshminer
