#! /bin/sh
groupadd -r virtualpen
usermod -aG virtuelpen $USER
echo 'KERNEL=="uinput", MODE="0777", GROUP="virtualpen", OPTIONS+="static_node=uinput"' | sudo tee /etc/udev/rules.d/60-android-virtual-pen.rules
udevadm control --reload
udevadm trigger
