# Difference Hash computation

Licensed under the [GNU General Public License, version 3. (GPLv3)](https://www.gnu.org/licenses/gpl-3.0.html)

Based on idea from [article](http://www.hackerfactor.com/blog/?/archives/529-Kind-of-Like-That.html) / [2](https://benhoyt.com/writings/duplicate-image-detection/)

## Behavior
- try to keep aspect ratio when computing hash
- use one of these 3 ratio that fit into 64bits: 8x8, 9x7, 10x6
- rotate image by 90 when needed to always fit into these ratios.
- use libvips thumbnail for fast reduction
- image formats are those supported by [libvips](https://www.libvips.org/)
- output results a one line json for easier script integration

## Compiling

Install dependencies:
- [libvips](https://www.libvips.org/)

{% highlight bash %}
meson build .
cd build
meson compile
{% endhighlight %}