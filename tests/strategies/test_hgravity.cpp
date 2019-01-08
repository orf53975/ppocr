/*
* Copyright (C) 2016 Wallix
* 
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
* 
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
* 
* You should have received a copy of the GNU Lesser General Public License along
* with this library; if not, write to the Free Software Foundation, Inc., 59
* Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#define BOOST_TEST_MODULE TestHorizontalGravity
#include <boost/test/auto_unit_test.hpp>

#include "ppocr/strategies/hgravity.hpp"
#include "ppocr/image/image_from_string.hpp"
#include "ppocr/image/image.hpp"


using namespace ppocr;

using D = strategies::hgravity::value_type;

namespace {
    strategies::hgravity hgravity;

    D to_hgravity_value(Bounds bnd, const char * data_text)
    {
        Image img = image_from_string(bnd, data_text);
        return hgravity.load(img, img/*.rotate90()*/);
    }
}

BOOST_AUTO_TEST_CASE(TestHGravity)
{
    D value;
    auto hInterval = hgravity.relationship().count() / 2;

    value = to_hgravity_value({3, 3},
        "---"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(value, hInterval);

    value = to_hgravity_value({3, 3},
        "-x-"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(value, hInterval*2);

    value = to_hgravity_value({3, 3},
        "---"
        "---"
        "-x-"
    );
    BOOST_CHECK_EQUAL(value, 0);

    value = to_hgravity_value({3, 3},
        "---"
        "-x-"
        "---"
    );
    BOOST_CHECK_EQUAL(value, hInterval);

    value = to_hgravity_value({3, 3},
        "--x"
        "x--"
        "--x"
    );
    BOOST_CHECK_EQUAL(value, hInterval);

    value = to_hgravity_value({3, 3},
        "--x"
        "---"
        "--x"
    );
    BOOST_CHECK_EQUAL(value, 100);

    value = to_hgravity_value({3, 5},
        "--x"
        "---"
        "---"
        "x-x"
        "--x"
    );
    BOOST_CHECK_EQUAL(value, (6 + 2 - 4) *100/6);

    auto const & relationship = hgravity.relationship();

    BOOST_CHECK_EQUAL(100, relationship(hInterval, hInterval));
    BOOST_CHECK_EQUAL(50,  relationship(hInterval*2, hInterval));
    BOOST_CHECK_EQUAL(75,  relationship(hInterval+hInterval/2, hInterval));
    BOOST_CHECK_EQUAL(75,  relationship(hInterval/2, hInterval));
    BOOST_CHECK_EQUAL(75,  relationship(hInterval/2, 0));
    BOOST_CHECK_EQUAL(25,  relationship(hInterval/2, hInterval*2));
    BOOST_CHECK_EQUAL(92,  relationship(hInterval/2, hInterval/3));
    BOOST_CHECK_EQUAL(79,  relationship(88, 45));
}
