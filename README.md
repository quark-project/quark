Quark Core integration/staging tree
===================================

[![Build Status](https://travis-ci.org/quark/quark.svg?branch=master)](https://travis-ci.org/quark/quark)

https://www.qrknet.info

What is Quark?
--------------

Quark is an experimental new digital currency that enables instant payments to
anyone, anywhere in the world. Quark uses peer-to-peer technology to operate
with no central authority: managing transactions and issuing money are carried
out collectively by the network. Quark Core is the name of open source
software which enables the use of this currency.

For more information, as well as an immediately useable, binary version of
the Quark Core software, see https://www.qrknet.info/.

License
-------

Quark Core is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see http://opensource.org/licenses/MIT.

Development process
-------------------

Developers work in their own trees, then submit pull requests when they think
their feature or bug fix is ready.

If it is a simple/trivial/non-controversial change, then one of the Quark
development team members simply pulls it.

If it is a *more complicated or potentially controversial* change, then the patch
submitter will be asked to start a discussion (if they haven't already) on the
[mailing list](https://lists.linuxfoundation.org/mailman/listinfo/quark-dev)

The patch will be accepted if there is broad consensus that it is a good thing.
Developers should expect to rework and resubmit patches if the code doesn't
match the project's coding conventions (see [doc/coding.md](doc/coding.md)) or are
controversial.

The `master` branch is regularly built and tested, but is not guaranteed to be
completely stable. [Tags](https://github.com/quark/quark/tags) are created
regularly to indicate new official, stable release versions of Quark.

Testing
-------

Testing and code review is the bottleneck for development; we get more pull
requests than we can review and test on short notice. Please be patient and help out by testing
other people's pull requests, and remember this is a security-critical project where any mistake might cost people
lots of money.

### Automated Testing

Developers are strongly encouraged to write unit tests for new code, and to
submit new unit tests for old code. Unit tests can be compiled and run (assuming they weren't disabled in configure) with: `make check`

Every pull request is built for both Windows and Linux on a dedicated server,
and unit and sanity tests are automatically run. The binaries produced may be
used for manual QA testing — a link to them will appear in a comment on the
pull request posted by [QuarkPullTester](https://github.com/QuarkPullTester). See https://github.com/TheBlueMatt/test-scripts
for the build/test scripts.

### Manual Quality Assurance (QA) Testing

Large changes should have a test plan, and should be tested by somebody other
than the developer who wrote the code.
See https://github.com/quark/QA/ for how to create a test plan.

Translations
------------

Changes to translations as well as new translations can be submitted to
[Quark Core's Transifex page](https://www.transifex.com/projects/p/quark/).

Translations are periodically pulled from Transifex and merged into the git repository. See the
[translation process](doc/translation_process.md) for details on how this works.

**Important**: We do not accept translation changes as GitHub pull requests because the next
pull from Transifex would automatically overwrite them again.

Translators should also subscribe to the [mailing list](https://groups.google.com/forum/#!forum/quark-translators).

Development tips and tricks
---------------------------

**compiling for debugging**

Run configure with the --enable-debug option, then make. Or run configure with
CXXFLAGS="-g -ggdb -O0" or whatever debug flags you need.

**debug.log**

If the code is behaving strangely, take a look in the debug.log file in the data directory;
error and debugging messages are written there.

The -debug=... command-line option controls debugging; running with just -debug will turn
on all categories (and give you a very large debug.log file).

The Qt code routes qDebug() output to debug.log under category "qt": run with -debug=qt
to see it.

**testnet and regtest modes**

Run with the -testnet option to run with "play quarks" on the test network, if you
are testing multi-machine code that needs to operate across the internet.

If you are testing something that can run on one machine, run with the -regtest option.
In regression test mode, blocks can be created on-demand; see qa/rpc-tests/ for tests
that run in -regtest mode.

**DEBUG_LOCKORDER**

Quark Core is a multithreaded application, and deadlocks or other multithreading bugs
can be very difficult to track down. Compiling with -DDEBUG_LOCKORDER (configure
CXXFLAGS="-DDEBUG_LOCKORDER -g") inserts run-time checks to keep track of which locks
are held, and adds warnings to the debug.log file if inconsistencies are detected.


How To Build
--------------

**Ubuntu 16.04**

```
# Install gcc-7 g++-7
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update 
sudo apt-get install gcc-7
sudo apt-get install g++-7

sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 100
sudo update-alternatives --config gcc
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 100
sudo update-alternatives --config g++

sudo apt-get update
sudo apt-get install git build-essential libtool autotools-dev automake pkg-config libevent-dev bsdmainutils python3
sudo apt-get install libboost-system-dev libboost-filesystem-dev libboost-chrono-dev libboost-program-options-dev libboost-test-dev libboost-thread-dev

sudo apt-get install software-properties-common
sudo add-apt-repository ppa:bitcoin/bitcoin
sudo apt-get update
sudo apt-get install libdb4.8-dev libdb4.8++-dev

sudo apt-get install libqrencode-dev
sudo apt-get install libqt5gui5 libqt5core5a libqt5dbus5 qttools5-dev qttools5-dev-tools libprotobuf-dev protobuf-compiler
sudo apt-get install libssl-dev


git clone https://github.com/mutalisk999/quark.git

cd quark
git checkout 0.10.7.5

sh autogen.sh
./configure --enable-tests=no --with-gui=no
make -j 4
```

**Ubuntu 18.04**

```
sudo apt-get update
sudo apt-get install git build-essential libtool autotools-dev automake pkg-config libevent-dev bsdmainutils python3
sudo apt-get install libboost-system-dev libboost-filesystem-dev libboost-chrono-dev libboost-program-options-dev libboost-test-dev libboost-thread-dev

sudo apt-get install software-properties-common
sudo add-apt-repository ppa:bitcoin/bitcoin
sudo apt-get update
sudo apt-get install libdb4.8-dev libdb4.8++-dev

sudo apt-get install libqrencode-dev
sudo apt-get install libqt5gui5 libqt5core5a libqt5dbus5 qttools5-dev qttools5-dev-tools libprotobuf-dev protobuf-compiler
sudo apt-get install libssl1.0-dev


git clone https://github.com/mutalisk999/quark.git

cd quark
git checkout 0.10.7.5

sh autogen.sh
./configure --enable-tests=no --with-gui=no
make -j 4
```

**Win64 (Cross-compilation on Ubuntu 18.04)**
```
sudo apt install build-essential libtool autotools-dev automake pkg-config bsdmainutils curl git cmake

sudo apt install g++-mingw-w64-x86-64
sudo update-alternatives --config x86_64-w64-mingw32-g++   
sudo update-alternatives --config x86_64-w64-mingw32-gcc    
# Select the option that includes `posix`, e.g. `/usr/bin/x86_64-w64-mingw32-g++-posix`

git clone https://github.com/mutalisk999/quark.git

cd quark
git checkout 0.10.7.5

PATH=$(echo "$PATH" | sed -e 's/:\/mnt.*//g')      
cd depends       
make HOST=x86_64-w64-mingw32 -j 4

cd ..       
sh autogen.sh      
CONFIG_SITE=$PWD/depends/x86_64-w64-mingw32/share/config.site ./configure --enable-tests=no --prefix=/         
make -j 4
```
