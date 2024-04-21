#! /bin/sh
groupadd -r virtualpen
usermod -aG virtualpen $USER
echo 'SUBSYSTEM=="usb", MODE="0777", GROUP="plugdev"\nKERNEL=="uinput", MODE="0777", GROUP="virtualpen", OPTIONS+="static_node=uinput"' | sudo tee /etc/udev/rules.d/60-android-virtual-pen.rules
udevadm control --reload
udevadm trigger
