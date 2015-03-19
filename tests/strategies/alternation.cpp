/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  Author(s): Jonathan Poelen
*/

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestAuthentifierNew
#include <boost/test/auto_unit_test.hpp>

#undef SHARE_PATH
#define SHARE_PATH FIXTURES_PATH

#define LOGNULL
//#define LOGPRINT

#include "strategies/alternation.hpp"
#include "image_from_string.hpp"
#include "image.hpp"
#include <sstream>

#define IMAGE_PATH "./images/"

BOOST_AUTO_TEST_CASE(TestAlternation)
{
    Image img = image_from_string({6, 7},
        "-xxxx-"
        "xx--xx"
        "xx--xx"
        "xxxxxx"
        "xx----"
        "xx--xx"
        "-xxxx-"
    );

    strategies::alternations alternations(img, img.rotate90());
    using seq_t = strategies::alternations::sequence_type;
    BOOST_CHECK(alternations[0] == seq_t({1, 0, 1}));
    BOOST_CHECK(alternations[1] == seq_t({1, 0}));
    BOOST_CHECK(alternations[2] == seq_t({1}));
    BOOST_CHECK(alternations[3] == seq_t({1}));
    BOOST_CHECK(alternations[4] == seq_t({1, 0, 1, 0, 1}));
    BOOST_CHECK(alternations[5] == seq_t({1}));
    BOOST_CHECK(alternations[6] == seq_t({1, 0, 1}));
    BOOST_CHECK_EQUAL(100, alternations.relationship(alternations));

    img = image_from_string({6, 7},
        "-x--x-"
        "xx--xx"
        "xx--xx"
        "xxxxxx"
        "xx----"
        "xx--xx"
        "-xxxx-"
    );

    strategies::alternations alternations2(img, img.rotate90());
    BOOST_CHECK(alternations2[0] == seq_t({1, 0, 1}));
    BOOST_CHECK(alternations2[1] == seq_t({1, 0}));
    BOOST_CHECK(alternations2[2] == seq_t({1, 0, 1}));
    BOOST_CHECK(alternations2[3] == seq_t({1}));
    BOOST_CHECK(alternations2[4] == seq_t({0, 1, 0, 1}));
    BOOST_CHECK(alternations2[5] == seq_t({1}));
    BOOST_CHECK(alternations2[6] == seq_t({1, 0, 1}));
    BOOST_CHECK_EQUAL(5*100/7, alternations.relationship(alternations2));
    BOOST_CHECK_EQUAL(5*100/7, alternations2.relationship(alternations));
}