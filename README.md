# Android Virtual Pen

Android Virtual Pen allows to send pen input data from your android device to a pc host where it emulates a virtual pen.
What distinguishes this solution from others, is emphasis on low-latency to provide the best drawing experience.

## Features

- Adjustable pen pressure threshold such that you can move the pen-cursor without triggering an unintended click/draw action
- Adjustable pen pressure sensitivity such that you can customize the force needed to be applied
- Eraser support
- Switch between stretched and fixed display mode
  - In stretched mode, the android device screen is stretched and completely mapped to your PC primary screen. Note that this is required to fully reach the boundaries of your PC screen from your device but causes some slight distortion if the screen ratios (android vs PC) are not the same.
  - In fixed mode, if the android device screen resolution fits into your PC screen, it is centered and mapped on that latter screen at a 1:1 ratio. This mode should incurr no distortion.
- Detect available usb devices and automatically save your settings for each
- Supports both X11 and Wayland

![alt text](https://github.com/Mathieu-Beliveau/android-virtual-pen/blob/main/android-virtual-pen-linux-host.png?raw=true)

## Limitations

- Can only target one screen on the PC host (primary screen)  
- Pen tilt is currently not supported (please do ask if you need this feature)
- No support on host side for MacOS and Windows

## Installation

### On Android

Install the following [APK](https://github.com/Mathieu-Beliveau/android-virtual-pen/releases/download/0.1/virtual-pen.apk)

### On Linux


#### Ubuntu 22.04

Download the following [.deb](https://github.com/Mathieu-Beliveau/android-virtual-pen/releases/download/0.1/virtual-pen-linux-host_0.1-1_amd64.deb).

Install with: `sudo apt install ./virtual-pen-linux-host_0.1-1_amd64.deb`



#### Flatpak

Until the software is out of beta stage, the flatpak package is distributed as a standalone bundle.

Download the [flatpak bundle](https://github.com/Mathieu-Beliveau/android-virtual-pen/releases/download/0.1/virtualpen.flatpak)

Assuming you have flatpak installed, run the installation with:

`flatpak install ./virtualpen.flatpak`

Note that if you do not have the right kde sdk already installed, this installation step might require some time.

Configure your system to allow your user to write to uinput by downloading the [setup script](https://github.com/Mathieu-Beliveau/android-virtual-pen/releases/download/0.1/setup.sh)

Make it executable with: `chmod +x ./setup.sh` and run it as a sudoer: `sudo ./setup.sh`

You can then launch the flatpak through: `flatpak run org.flatpak.virtualpen`


#### Standalone Binary

The [standalone binary](https://github.com/Mathieu-Beliveau/android-virtual-pen/releases/download/0.1/virtual-pen-linux-host) has been compiled against Qt 6.7 and requires libusb-1.0. Make sure these are available on your system.

Configure your system to allow your user to write to uinput by downloading the [setup script](https://github.com/Mathieu-Beliveau/android-virtual-pen/releases/download/0.1/setup.sh)

Make it executable with: `chmod +x ./setup.sh` and run it as a sudoer: `sudo ./setup.sh`


## Usage

Start the android application connected to your PC through usb. 

It should warn that the link with the PC is not yet established. 

For the link to be fully establish, you need to start the virtual-pen app on your PC which will set your device in accessory mode.

On the linux host, launch the Virtual Pen application, select your device (hit refresh if not shown) then enter the device's screen horizontal and vertical resolutions and then press "connect". Upon the first connection, Android should ask you to allow the Virtual Pen app to handle the connection - press ok. 
Once this done, you should be able to use your pen and see your PC cursor move as intended.

Note that this software is still in beta stage and still ought to be tested with various devices.

## Building

For Android, the app has been build with the SDK Android 14 (API Level 34) and should readily build in Android Studio.

For the linux-host, if you are not setup for building C++ and cmake projects, begin with:

``sudo apt-get install build-essential cmake``

install the required dependencies through:

```
sudo apt-get install libusb-1.0-0-dev libgl1-mesa-dev qt6-base-dev
```

Then into the sources directory ``virtual-pen-linux-host``, run:

``cmake ./`` to generate the cmake build.

Then peform the actual build through:

``make``

## Special Thanks

This project was inspired by the very excellent [Weylus](https://github.com/H-M-H/Weylus) application and
made significant use of the [Linux ADK](https://github.com/gibsson/linux-adk). Thanks to both devs and contributors!


