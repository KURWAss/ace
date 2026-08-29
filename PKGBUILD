# Maintainer: KURWA <kurwa.offc@proton.me>

pkgname=ace-git
pkgver=r1.0000000
pkgrel=1
pkgdesc="A minimal floating window manager for X11, written in C++ (git version)"
arch=('x86_64')
url="https://github.com/KURWAss/ace"
license=('GPL3')
depends=('libx11')
makedepends=('git' 'gcc' 'make')
provides=('ace')
conflicts=('ace')
source=("$pkgname::git+https://github.com/OWNER/ace.git")
sha256sums=('SKIP')

pkgver() {
    cd "$pkgname"
    printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

build() {
    cd "$pkgname"
    make
}

package() {
    cd "$pkgname"
    install -Dm755 ace "$pkgdir/usr/bin/ace"
    install -Dm644 README.md "$pkgdir/usr/share/doc/$pkgname/README.md"
    install -Dm644 CONTRIBUTING.md "$pkgdir/usr/share/doc/$pkgname/CONTRIBUTING.md"
}
