# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI=2
inherit cmake-utils git-2

DESCRIPTION="Image hashing library for C++"
HOMEPAGE="https://github.com/shuttie/libimghash"
SRC_URI=""
EGIT_REPO_URI="https://github.com/shuttie/libimghash.git"

DEPEND="media-gfx/imagemagick[cxx]"

LICENSE="LGPL-3"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE="" 
