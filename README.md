Video HOW-TO

https://www.youtube.com/watch?v=dkIUxUBbn6U

# Overview

![image](https://user-images.githubusercontent.com/35274849/209229932-9f8a440a-cd51-471e-9da5-70bdc068e7aa.png)

# Uploading the code

**Note: If you prefer to use Arduino IDE, just take the \src\main.cpp file and rename it to "something".ino (for example CubeCell_GPS_Helium_Mapper.ino)**

Install Serial Driver. https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads

Install [Visual Studio Code](https://code.visualstudio.com/Download). If you are using Windows, make sure the pick the System installer, not the User installer.

(Optional) When the Get Started wizard prompts you to install language extensions - install the C/C++ extension.

Install Git from https://git-scm.com/downloads or https://github.com/git-guides/install-git

Reboot your computer for the path changes to take effect.

Install the GitHub Pull Requests and Issues extension from the link [here](https://code.visualstudio.com/docs/editor/github).

Install [PlatformIO IDE](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide)

Once you are in Visual Studio Code, go to the Explorer and click Clone Repository. Paste the URL you got from GitHub, by clicking on the Code button. When prompted for location, if you want to use the default location (and you are using Windows) do the following - select your Documents folder, if there is no PlatformIO sub-folder in it - create one and enter it, then if there is no Projects sub-folder inside - create it and select it as the location for the cloned repository. So the final location would be %userprofile%\Documents\PlatformIO\Projects

Open the cloned folder

Open the main.cpp from src sub-folder and wait. Initially the #include directives at the top will have squiggly lines as unknown, but relatively soon (within 5 min) PlatformIO will detect and install the required platform and libraries. If you don't want to wait, open PlatformIO and go to Platforms and install "ASR Microelectronics ASR650x". You can do that as a step right after installing PlatformIO.

Comment out/uncomment the appropriate line for your board version (for GPS Air530 or Air530Z) in main.cpp.

Comment out/uncomment the #define lines for VIBR_SENSOR, VIBR_WAKE_FROM_SLEEP, MENU_SLEEP_DISABLE_VIBR_WAKEUP, MAX_GPS_WAIT, MAX_STOPPED_CYCLES and edit the values for the timers if desired.

Enter DevEUI(msb), AppEUI(msb), and AppKey(msb) from Helium Console, at the respective places in main.cpp. The values must be in MSB format. From console press the expand button to get the ID's as shown below.

![Console Image](https://gblobscdn.gitbook.com/assets%2F-M21bzsbFl2WA7VymAxU%2F-M6fLGmWEQ0QxjrJuvoC%2F-M6fLi5NzuMeWSzzihV-%2Fcubecell-console-details.png?alt=media&token=95f5c9b2-734a-4f84-bb88-523215873116)

```
uint8_t devEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
```

Modify platformio.ini if you need to change LoRaWAN settings like region.

Click the PlatformIO: Build button. Address any compile errors and repeat until you get a clean build.

Connect the CubeCell to the computer with USB cable.

Click the PlatformIO: Upload button.

# Debug using Serial connection via USB

(Optional) Uncomment the line enabling the DEBUG code and build again.
```
//#define DEBUG // Enable/Disable debug output over the serial console
```
Click the PlatformIO: Serial Monitor button

# Setting up Console

In [Helium Console](https://console.helium.com/) create a new function call it Heltec decoder => Type Decoder => Custom Script

Copy and paste the decoder into the custom script pane :

https://github.com/1rabbit/HTCC-AB02S-Universal-Sensor/blob/master/rabbit_universal_decoder.js



