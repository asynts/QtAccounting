pkgname="asynts-accounting"
pkgver="0.4.0"
pkgrel="1"
arch=("x86_64")
url="https://github.com/asynts/QtAccounting"

source=("https://github.com/asynts/QtAccounting/archive/refs/tags/version/0.4.0.tar.gz")
sha256sums=("11324904407d66b9ac813fb54bb8584d8331d617105684c23ad6cd9b9923f209")

build() {
    cmake \
        -B build \
        -S "QtAccounting-version-${pkgver}" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr

    cmake \
        --build build
}

package() {
    DESTDIR="${pkgdir}" \
    cmake \
        --install build
}
