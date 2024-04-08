# Android Virtual Pen

Android Virtual Pen allows to send pen input data from your android device to a pc host where it emulates a virtual pen.
What distinguishes this solution from others, is emphasis on low-latency to provide the best drawing experience.

Current features:

- Adjustable pen pressure threshold such that you can move the pen-cursor without triggering an unintended click/draw action
- Adjustable pen pressure sensitivity such that you can customize the force needed to be applied
- Switch between stretched and fixed display mode
  - In stretched mode, the android device screen is stretched and completely mapped to your PC primary screen. Note that this is required to fully reach the boundaries of your PC screen from your device but causes some slight distortion if the screen ratios (android vs PC) are not the same.
  - In fixed mode, if the android device screen resolution fits into your PC screen, it is centered and mapped on that latter screen at a 1:1 ratio. This mode should incurr no distortion.
- Detect available usb devices and automatically save your settings for each

![alt text](https://github.com/Mathieu-Beliveau/android-virtual-pen/blob/main/image.jpg?raw=true)

Limitations:

- Can only target one screen on the PC host (primary screen)  
- Pen tilt is currently not supported (please do ask if you need this feature)
- No support on host side for MacOS and Windows



