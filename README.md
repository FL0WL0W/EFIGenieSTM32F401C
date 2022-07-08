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
***TODO, setup probably similar to Linux
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
2. Add the bin folder to your environment path. To do this, type "Environment Variable" into the start menu and select "Edit the system environment variables" 
![image](https://user-images.githubusercontent.com/1595263/178067947-d407263f-26bd-4a2c-a7dd-f2b12f015fdd.png)
3. Click the button "Environment Variables..." 
![image](https://user-images.githubusercontent.com/1595263/178068056-b4ab842b-89ca-4845-9d64-a80192caaa93.png)
4. Select the Path variable and click "Edit" 
![image](https://user-images.githubusercontent.com/1595263/178068336-3e503a8e-2917-4659-aa43-eae185bc596f.png)
5. Select "New" and enter the location of the OpenOCD bin. In this case C:\OpenOCD\bin 
![image](https://user-images.githubusercontent.com/1595263/178068539-0fa86af0-9468-4c9e-af4e-9b083819332f.png)
