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
3. Before clicking finish, select "Add path to environment variable." If this step is skipped, you will need to add the path for the toolchain bin manually to your environment variable
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
