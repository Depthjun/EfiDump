# EfiDump
Yet another PoC EFI runtime driver. This time for direct process memory read/write. This is a simple example of how can process dumper work. If you want to use this for some more complex project, please add memory checks (or you are going to be bluescreening every 5 minutes) and save the Windows kernel exports pointers in the driver once, so you don't have to send them every time. [Here is a video of it in action](https://youtu.be/cTxCyzFaa20).

## Compiling
First you need a working Linux install (or you can use Linux subsystem for Windows) and install gnu-efi (commands for Ubuntu 20.04):
```
sudo apt install gnu-efi build-essential
```
That's all you need to install. Package manager (in the example apt) should take care of all the depencies for you. Once the installation is complete, clone this repo (make sure you have git installed):
```   
git clone https://github.com/SamuelTulach/EfiDump
```
Than navigate to the driver folder and compile the driver with make:
```
cd EfiDump
cd driver
cmake ..
make
```
If the compile was successful, you should now see driver.efi in the driver folder. Same applies to client.

## Usage
In order to use the efi-memory driver, you need to load it. First, obtain a copy of driver.efi ([compile it](https://github.com/SamuelTulach/EfiDump#compiling) or [download it from release section](https://github.com/SamuelTulach/EfiDump/releases/)) and a copy of [EDK2 efi shell](https://github.com/tianocore/edk2/releases). Now follow these steps:

1. Extract downloaded efi shell and rename file Shell.efi (should be in folder UefiShell/X64) to bootx64.efi
2. Format some USB drive to FAT32
3. Create following folder structure:
```
USB:.
 │   driver.efi
 │
 └───EFI
      └───Boot
              bootx64.efi
```
4. Boot from the USB drive
5. An UEFI shell should start, change directory to your USB (FS0 should be the USB since we are booting from it) and list files:
```
FS0:
ls
```
6. You should see file driver.efi, if you do, load it:
```
load driver.efi
```
7. Now you should see log and confirmation text. Boot into the OS. While booting you should see a blue background on the bootscreen which indicates that the driver is running.

## Thanks
- [EfiGuard](https://github.com/Mattiwatti/EfiGuard) for runtime services hooking idea
- [kdmapper](https://github.com/z175/kdmapper) for kernel exports resolving
- [TheCruZ](https://www.unknowncheats.me/forum/members/1117395.html) for telling me that I can specify MS ABI in GCC instead of using mingw