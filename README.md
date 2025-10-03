# Getting started
## Get a local copy of the repository
```
cd <my-devel-ert-dir>
git clone https://github.com/EPFLRocketTeam/2024_C_AV_RPI.git
```
Then to get the latest version of the submodules, run:
```
git submodule update --init
```
## Start contributing
You want to add a new feature or correct a bug to the main branch ? 
- Create a new branch (from main) and publish it:
  ```
  git checkout -b <new-feat-branch>
  git push -u origin <new-feat-branch>
  ```
- Open a pull request on github, from `<new-feat-branch>` to `main`. If you want to be more communicative about the advancement of your task, you can in a first time declare the PR as a draft. When you think your changes are worth being implemented, you can "undraft" it.
- Start bringing changes on your new branch.
- After each commit/push, make sure that your PR can be merged without conflicts, i.e. its status is "able to merge". If a conflict is detected, please don't add any more changes, and try to resolve it (we have faith in you).
- You can check wether your modifications compile on the remote Rpi, by observing the result of the automatic tests, shown at the bottom of your PR. You can also verify it by yourself, by connecting to the Rpi:<br>
  Once connected,
  ```
  cd Documents/2024_C_AV_RPI
  git checkout <new-feat-branch>
  git pull
  ```
  Then, build!
## Build the code
To build the code, you will have to connect to the Rpi via SSH.
Once in the repository, execute: 
```
cd build
make
```

Then you can either run the tests individually with:
```
sudo ./build/tests/[NAME_OF_TEST]_test
```

Or run all the tests with:
```
sudo ctest
```

### A workaround to run gdb with sudo:
```
cd /usr/bin
sudo mv gdb gdborig
```
Then make a bash script named gdb:
```
sudo nano gdb
```
And modify it with the following content:
```
#!/bin/sh
sudo gdborig #@
```
Finally make the script runnable
```
sudo chmod 0755 gdb
```
Then you can debug any executable by selecting it in the CMake Tools window, and
clicking on the executable name under debug.


script| workaround
