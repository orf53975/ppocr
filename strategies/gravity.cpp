#include "gravity.hpp"
#include "image.hpp"

#include <ostream>
#include <istream>

namespace strategies
{

static inline int normalize_positif_gravity(const Image& img, unsigned d, int plus)
{
    if (d > img.height() / 4) {
        if (d > img.height()) {
            return plus+2;
        }
        return plus+1;
    }
    return 0;
}

static unsigned count_gravity(Bounds const & bnd, Pixel const * p, Pixel const * ep, bool is_top)
{
    unsigned g = 0;
    unsigned coef = is_top ? bnd.h() : 1;
    while (p != ep) {
        for (auto epl = p + bnd.w(); p != epl; ++p) {
            if (is_pix_letter(*p)) {
                g += coef;
            }
        }

        if (is_top) {
            --coef;
        }
        else {
            ++coef;
        }
    }
    return g;
}

static int horizontal_gravity(const Image& img)
{
    Bounds const bnd(img.width(), img.height() / 2);
    auto p = img.data();
    auto ep = img.data({0, bnd.h()});
    unsigned const top = count_gravity(bnd, p, ep, true);
    p = ep;
    if (img.height() & 1) {
        p += img.width();
    }
    unsigned const bottom = count_gravity(bnd, p, img.data_end(), false);

    return (top > bottom) ? normalize_positif_gravity(img, top - bottom, 0)
        : (top < bottom) ? normalize_positif_gravity(img, bottom - top, 2)
        : 0;
}

gravity::gravity(const Image& img, const Image& img90)
: d(horizontal_gravity(img) + horizontal_gravity(img90) * 5)
{}


std::istream& operator>>(std::istream& is, gravity& d)
{ return is >> d.d; }

std::ostream& operator<<(std::ostream& os, const gravity& d)
{ return os << d.id(); }

}
