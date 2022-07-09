# EFIGenieSTM32F401C

# Development Environment Setup
VSCode is the IDE used for developing EFIGenie as well as the STM32 branches. The recommneded extensions are listed in .vscode/extensions.json and when opening the project in VSCode it will prompt you to install these extensions if you haven't already
In addition to VSCode, you will also need the ARM Embedded Toolchain and a cmake generator to compile the project. The CMake generator typically used is ninja
## VSCode
Download VSCode from <a href="https://code.visualstudio.com/download">here</a>. The installation is straight forward and additional information for setting up VSCode can be found <a href="https://code.visualstudio.com/docs/setup/setup-overview">here</a>.

## ARM Embedded Toolchain
Download the toolchain from <a href="https://developer.arm.com/downloads/-/gnu-rm">here</a>. Choose the correct download for your operating system. 
### Windows
1. For windows, download the toolchain installer executable from the link above. It will be the download with extension "win32.exe"
2. Open the executable and follow the prompts for your language and EULA.
3. Before clicking finish, select "Add path to environment variable." If this step is skipped, you will need to add the path for the toolchain bin manually to your environment path. Instructions for how to add the path to your environment path can be found below in the installation for OpenOCD
### Linux
***need to update***
### Mac
M1 Mac does not work with the lates arm toolchain so you will need to download 10.3-2021.07 instead of 10.3-2021.10
***TODO, setup probably similar to Linux

## CMake
Download and install CMake from <a href="https://cmake.org/download/">here</a>.

## CMake Generator
Download Ninja from the releases <a href="https://github.com/ninja-build/ninja/releases">here</a>.
### Windows
1. Unzip Ninja
2. You will need to place this in your environment path, the easiest way to do this is to place it in the same folder as your arm toolchain bin which can be found here C:\Program Files (x86)\GNU Arm Embedded Toolchain\10 2021.10\bin
### Linux
***need to update***
### Mac
***TODO, setup probably similar to Linux
## Debugging
OpenOCD is used in conjunction with an ST-Link V2 adapter. OpenOCD binaries can be found <a href="https://openocd.org/pages/getting-openocd.html">here</a>.
### Windows
1. Extract the zip file to a directory on your machine. In my case C:\OpenOCD
2. Add the bin folder to your environment path. To do this, type "Environment Variable" into the start menu and select "Edit the system environment variables" <br>
![image](https://user-images.githubusercontent.com/1595263/178067947-d407263f-26bd-4a2c-a7dd-f2b12f015fdd.png)
3. Click the button "Environment Variables..." <br>
![image](https://user-images.githubusercontent.com/1595263/178068056-b4ab842b-89ca-4845-9d64-a80192caaa93.png)
4. Select the Path variable and click "Edit" <br>
![image](https://user-images.githubusercontent.com/1595263/178068336-3e503a8e-2917-4659-aa43-eae185bc596f.png)
5. Select "New" and enter the location of the OpenOCD bin. In this case C:\OpenOCD\bin <br>
![image](https://user-images.githubusercontent.com/1595263/178068539-0fa86af0-9468-4c9e-af4e-9b083819332f.png)

# Cloaning the project
1. Clone this repository and then run the following git commands to pull in the dependencies. On linux you can just run the gitinit.sh script, not sure how to do that on windows.
```
git submodule init
git submodule update
cd libs/EFIGenie
git submodule init
git submodule update
```
2. Open folder in VSCode and install the recommended extensions that it prompts you when opening the project.

# Building the project
1. Select the configuration and build presets at the bottom of vscode <br>
![image](https://user-images.githubusercontent.com/1595263/178071479-d35481fd-5966-4e3f-88c8-59214be06b3a.png) <br>
![image](https://user-images.githubusercontent.com/1595263/178071514-4f2e798e-24a5-4e3c-b3a0-7c2c57784292.png)
2. Navigate to the Cmake extension and click "Configure All Projects" followed by "Build All Projects" <br>
![image](https://user-images.githubusercontent.com/1595263/178071657-1a0ec013-d803-4eef-9ece-52fb00a90849.png)

# Debugging the project
1. Build the project
2. Connect the STM32F401 to the ST-Link V2 and then to your computer
3. Navigate to the Cortex-Debug extension and click "Start Debugging" This will load the build to the STM32F401 <br>
![image](https://user-images.githubusercontent.com/1595263/178072643-500e261f-9772-4512-8d4f-39802603f0bb.png)

# Running the project without an ST-Link
1. Build the project
2. In the build folder there is both a firmware.bin and firmware.hex file which can be loaded using the STM32CubeProgrammer from ST. STM32CubeProgrammer can be connected to the STM32F401 in USB bootloader mode by holding the boot pin load during reset. STM32CubeProgrammer can be downloaded <a href="https://www.st.com/en/development-tools/stm32cubeprog.html#get-software">here</a>.
3. Navigate to the Erasing and Programming tab and select the firmware file and click "Start Programming" <br>
![image](https://user-images.githubusercontent.com/1595263/178074897-7f7d68ad-5cc3-4730-8d61-423ffc6df184.png)


# Loading a tune
Burning the tune from the GUI is currently in progress so tunes must loaded by burning the tune.bin using STM32CubeProgrammer
1. Use EFIGenieEditor to generate a bin file by clicking "Save Bin"
2. Connect STM32CubeProgrammer to the STM32F401 using either an ST-Link or the USB bootloader
3. Navigate to the Erasing and Programming tab and select the tune file, set the Start Address to 0x08004000 and click "Start Programming" <br> 
![image](https://user-images.githubusercontent.com/1595263/178075027-87899c8e-7bdb-456b-8459-4d813a76b1bd.png)


