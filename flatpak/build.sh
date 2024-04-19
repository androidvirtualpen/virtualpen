#! /bin/sh
flatpak run org.flatpak.Builder --force-clean --sandbox --user --install --ccache --mirror-screenshots-url=https://dl.flathub.org/media/ --repo=repo builddir ./io.github.androidvirtualpen.virtualpen.json
ostree commit --repo=repo --canonical-permissions --branch=screenshots/x86_64 builddir/files/share/app-info/media
echo "Done building \n\n"
echo "Running linter for repo \n"
flatpak run --command=flatpak-builder-lint org.flatpak.Builder repo repo
echo "Running linter for manifest \n"
flatpak run --command=flatpak-builder-lint org.flatpak.Builder manifest io.github.androidvirtualpen.virtualpen.json
echo "Generating standalone bundle \n"
flatpak build-export export builddir
flatpak build-bundle export virtualpen.flatpak io.github.androidvirtualpen.virtualpen --runtime-repo=https://flathub.org/repo/flathub.flatpakrepo
