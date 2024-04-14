#! /bin/sh
rm -rf ./build-dir
rm -rf ./export
flatpak-builder --disable-cache --force-clean build-dir ./org.flatpak.virtualpen.json
flatpak build-export export build-dir
flatpak build-bundle export virtualpen.flatpak org.flatpak.virtualpen --runtime-repo=https://flathub.org/repo/flathub.flatpakrepo 
flatpak-builder --install --force-clean build-dir ./org.flatpak.virtualpen.json
