# Android Virtual Pen

Android Virtual Pen allows to send pen input data from your android device to a pc host where it emulates a virtual pen.
What distinguishes this solution from others, is emphasis on low-latency to provide the best drawing experience.

## Features

- Adjustable pen pressure threshold such that you can move the pen-cursor without triggering an unintended click/draw action
- Adjustable pen pressure sensitivity such that you can customize the force needed to be applied
- Switch between stretched and fixed display mode
  - In stretched mode, the android device screen is stretched and completely mapped to your PC primary screen. Note that this is required to fully reach the boundaries of your PC screen from your device but causes some slight distortion if the screen ratios (android vs PC) are not the same.
  - In fixed mode, if the android device screen resolution fits into your PC screen, it is centered and mapped on that latter screen at a 1:1 ratio. This mode should incurr no distortion.
- Detect available usb devices and automatically save your settings for each

![alt text](https://github.com/Mathieu-Beliveau/android-virtual-pen/blob/main/android-virtual-pen-linux-host.png?raw=true)

## Limitations

- Can only target one screen on the PC host (primary screen)  
- Pen tilt is currently not supported (please do ask if you need this feature)
- No support on host side for MacOS and Windows

## Installation

For Android, install the following APK:

https://github.com/Mathieu-Beliveau/android-virtual-pen/releases/download/0.1/virtual-pen.apk

On Linux:

For debian-based distros, install the following .deb:

https://github.com/Mathieu-Beliveau/android-virtual-pen/releases/download/0.1/virtual-pen-linux-host_0.1-1_amd64.deb

Or place the following file on your PATH:

https://github.com/Mathieu-Beliveau/android-virtual-pen/releases/download/0.1/virtual-pen-linux-host

If you are installing through the compiled binary (not the .deb), note that that the pen virtualization 
is achieved through `uinput` which implies that `/dev/uinput` must be writeable by the user running the host application.

```
sudo groupadd -r uinput
sudo usermod -aG uinput $USER
echo 'KERNEL=="uinput", MODE="0660", GROUP="uinput", OPTIONS+="static_node=uinput"' \
| sudo tee /etc/udev/rules.d/60-android-virtual-pen.rules
```

Then, either reboot, or run:

```
sudo udevadm control --reload
sudo udevadm trigger
```

then log out and log. To restore your system in its previous state, simply run

``sudo rm /etc/udev/rules.d/60-android-virtual-pen.rules``

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


