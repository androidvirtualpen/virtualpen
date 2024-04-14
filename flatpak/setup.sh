#! /bin/sh
groupadd -r uinput
usermod -aG uinput $USER
echo 'KERNEL=="uinput", MODE="0660", GROUP="uinput", OPTIONS+="static_node=uinput"' | sudo tee /etc/udev/rules.d/60-android-virtual-pen.rules
udevadm control --reload
udevadm trigger
