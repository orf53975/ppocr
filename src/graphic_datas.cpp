/*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*   Product name: redemption, a FLOSS RDP proxy
*   Copyright (C) Wallix 2010-2015
*   Author(s): Jonathan Poelen
*/

#include "factory/data_loader.hpp"
#include "factory/definition.hpp"
#include "factory/registry.hpp"

#include "strategies/utils/basic_proportionality.hpp"
#include "strategies/hdirection.hpp"
#include "strategies/hdirection2.hpp"
#include "strategies/hgravity.hpp"
#include "strategies/hgravity2.hpp"
#include "strategies/proportionality.hpp"
#include "strategies/dvdirection.hpp"
#include "strategies/dvdirection2.hpp"
#include "strategies/dvgravity.hpp"
#include "strategies/dvgravity2.hpp"
#include "strategies/density.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <algorithm>

#include <cstring>
#include <cerrno>


int main(int ac, char ** av) {
    if (ac < 1) {
        std::cerr << av[0] << " datas\n";
        return 1;
    }

    std::ifstream file(av[1]);
    if (!file) {
        std::cerr << strerror(errno) << '\n';
        return 2;
    }

    DataLoader loader;
    all_registy(loader);

    std::vector<Definition> definitions = read_definitions(file, loader);

    if (!file.eof()) {
        std::cerr << "read error\n";
        return 5;
    }

    std::cout << "definitions.size = " << definitions.size() << "\n\n";

    struct Column { std::string c; unsigned value; };
    std::vector<Column> columns;

    struct LessColumnValue {
        LessColumnValue() {}
        bool operator()(Column const & c1, Column const & c2) const noexcept
        { return c1.value < c2.value; }
    };

    std::vector<std::vector<std::reference_wrapper<std::string const>>> characters_by_columns;

    unsigned coef[] = {
        strategies::hdirection::traits::get_interval(),
        strategies::hdirection90::traits::get_interval(),
        strategies::hdirection2::traits::get_interval(),
        strategies::hdirection290::traits::get_interval(),
        strategies::hgravity::traits::get_interval(),
        strategies::hgravity90::traits::get_interval(),
        strategies::hgravity2::traits::get_interval(),
        strategies::hgravity290::traits::get_interval(),
        strategies::proportionality::traits::get_interval(),
        strategies::dvdirection::traits::get_interval(),
        strategies::dvdirection90::traits::get_interval(),
        strategies::dvdirection2::traits::get_interval(),
        strategies::dvdirection290::traits::get_interval(),
        strategies::dvgravity::traits::get_interval(),
        strategies::dvgravity90::traits::get_interval(),
        strategies::dvgravity2::traits::get_interval(),
        strategies::dvgravity290::traits::get_interval(),
        strategies::density::traits::get_interval(),
        0,
        0
    };

    for (size_t i = 0; i < 20; ++i) {
        for (Definition const & def : definitions) {
            DataLoader::Datas const & datas = def.datas;
            using Data = DataLoader::data_base;
            columns.push_back(Column{def.c, *reinterpret_cast<unsigned const *>(
                reinterpret_cast<unsigned char const *>(&datas[i]) + sizeof(Data)
            )});
        }

        auto const minmax = std::minmax_element(columns.cbegin(), columns.cend(), LessColumnValue{});
        unsigned const min = minmax.first->value;
        unsigned const max = minmax.second->value;

        characters_by_columns.clear();
        characters_by_columns.resize(max - min + 1);

        std::cout << "\n ## " << loader.names()[i] << "\n   [ " << min << "-" << max << " ]\n";

        std::cout.fill('-');
        right(std::cout);
        char const * colors[]{
//             "\033[07;31mx\033[0m",
//             "\033[07;32mx\033[0m",
            "\033[07;33mx\033[0m",
            "\033[07;34mx\033[0m",
//             "\033[07;35mx\033[0m",
        };
        unsigned icolor = 0;
        unsigned dmax = 0;
        auto first = columns.begin();
        while (first != columns.end()) {
            auto last = std::find_if(
                first+1, columns.end(),
                [first](Column const & c) { return c.c != first->c; }
            );
            std::sort(first, last, LessColumnValue{});
            auto const dmax_tmp = (last-1)->value - first->value;
            auto const dmin_tmp = std::inner_product(
                first+1, last, first, 0u,
                [](unsigned a, unsigned b) { return std::min(a, b); },
                [](Column const & c1, Column const & c2) { return c2.value - c1.value; }
            );

            std::cout << "  # dmin: " << dmin_tmp << "  dmax: " << dmax_tmp << "\n";
            for (; first != last; ++first) {
                std::cout
                  << std::setw(first->value) << ""
                  << colors[icolor]
                  << std::setw(max - first->value) << ""
                  << " " << first->c << "\n"
                ;
                characters_by_columns[first->value - min].push_back(first->c);
            }
            icolor = (icolor + 1) % (sizeof(colors)/sizeof(colors[0]));
            dmax = std::max(dmax_tmp, dmax);
        }

        std::cout << "\n";

        std::cout.fill(' ');
        left(std::cout);
        unsigned n = min;
        for (auto & characters : characters_by_columns) {
            std::cout << std::setw(4) << n++ << " ";
            for (std::string const & c : characters) {
                std::cout << c << " ";
            }
            std::cout << "\n";
        }

        std::cout << " # dmax: " << dmax << "   pmax: ";
        if (coef[i]) {
            std::cout << "+-" << (dmax * 50 / coef[i]) << "\n";
        }
        else {
            std::cout << "???\n";
        }

        columns.clear();
    }


//     std::string s;
//     while (std::cout << "> ") {
//         std::cin >> s;
//
//         if (std::cin.eof()) {
//             break;
//         }
//
//         if (!std::cin) {
//             std::cin.clear();
//             continue;
//         }
//
//         auto pos = std::lower_bound(
//             definitions.begin(), definitions.end(), s,
//             [](Definition const & def, std::string const & s) {
//                 return def.c < s;
//             }
//         );
//
//         columns.clear();
//         for (; pos != definitions.end() && pos->c == s; ++pos) {
//             std::cout << pos->img;
//             DataLoader::Datas const & datas = pos->datas;
//             for (size_t i = 0; i < 1/*20*/; ++i) {
//                 using Data = DataLoader::data_base;
//                 columns.push_back(Column{pos->c, *reinterpret_cast<unsigned const *>(
//                     reinterpret_cast<unsigned char const *>(&datas[i]) + sizeof(Data)
//                 )});
//             }
//         }
//
//         std::sort(
//             columns.begin(), columns.end(),
//             [](Column const & c1, Column const & c2) { return c1.c < c2.c; }
//         );
//
//         unsigned max = std::max_element(
//             columns.begin(), columns.end(),
//             [](Column const & c1, Column const & c2) { return c1.value < c2.value; }
//         )->value;
//         std::cout << "max: " << max << "\n";
//
//         std::cout.fill('-');
//         for (Column & c : columns) {
//             std::cout << std::setw(c.value) << "" << "x" << std::setw(max-c.value) << "-" << c.c <<  "\n";
//         }
//
//         std::cout << "\n";
//     }
}
