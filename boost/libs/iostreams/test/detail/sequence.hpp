// (C) Copyright Jonathan Turkanis 2004
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

#ifndef BOOST_IOSTREAMS_TEST_SEQUENCE_HPP_INCLUDED
#define BOOST_IOSTREAMS_TEST_SEQUENCE_HPP_INCLUDED

#include <algorithm>
#include <iterator>
#include <string.h>            // strlen.
#include <vector>
#include "./constants.hpp"

namespace boost { namespace iostreams { namespace test {

template<typename Ch = char>
struct test_sequence : public std::vector<Ch> {
    test_sequence()
        {
            this->reserve(data_reps * data_length());
            for (int z = 0; z < data_reps; ++z)
                std::copy( test::data<Ch>(), test::data<Ch>() + data_length(),
                           std::back_inserter(*this) );
        }
};

struct text_sequence : public std::vector<char> {
    text_sequence()
        {
            const char* c = 
                "The Lord spoke to Moses saying: Speak to the children of "
                "Israel, and have them take for Me an offering; from every "
                "person whose heart inspires him to generosity, you shall take "
                "My offering. And this is the offering that you shall take "
                "from them: gold, silver, and copper; blue, purple, and "
                "crimson wool; linen and goat hair; ram skins dyed red, "
                "tachash skins, and acacia wood; oil for lighting, spices for "
                "the anointing oil and for the incense; shoham stones and "
                "filling stones for the ephod and for the choshen. And they "
                "shall make Me a sanctuary and I will dwell in their midst "
                "according to all that I show you, the pattern of the Mishkan "
                "and the pattern of all its vessels; and so shall you do. They "
                "shall make an ark of acacia wood, two and a half cubits its "
                "length, a cubit and a half its width, and a cubit and a half "
                "its height. And you shall overlay it with pure gold; from "
                "inside and from outside you shall overlay it, and you shall "
                "make upon it a golden crown all around. And you shall cast "
                "four golden rings for it, and you shall place them upon its "
                "four corners, two rings on its one side, and two rings on its "
                "other side. And you shall make poles of acacia wood and you "
                "shall overlay them with gold. And you shall bring the poles "
                "into the rings on the sides of the ark, to carry the ark with "
                "them. The poles of the ark shall be in the rings; they shall "
                "not be removed from it. And you shall place into the ark the "
                "testimony, which I will give you. "
                "And you shall make an ark cover of pure gold, two and a half "
                "cubits its length and a cubit and a half its width. And you "
                "shall make two golden cherubim; you shall make them of "
                "hammered work, from the two ends of the ark cover. And make "
                "one cherub from the one end and the other cherub from the "
                "other end; from the ark cover you shall make the cherubim on "
                "its two ends. The cherubim shall have their wings spread "
                "upwards, shielding the ark cover with their wings, with their "
                "faces toward one another; [turned] toward the ark cover shall "
                "be the faces of the cherubim. And you shall place the ark "
                "cover on the ark from above, and into the ark you shall place "
                "the testimony, which I will give you. I will arrange My "
                "meetings with you there, and I will speak with you from atop "
                "the ark cover from between the two cherubim that are upon the "
                "Ark of the Testimony, all that I will command you unto the "
                "children of Israel. And you shall make a table of acacia "
                "wood, two cubits its length, one cubit its width, and a cubit "
                "and a half its height. And you shall overlay it with pure "
                "gold, and you shall make for it a golden crown all around. "
                "And you shall make for it a frame a handbreadth [wide] all "
                "around, and you shall make a golden crown for its frame all "
                "around. And you shall make for it four golden rings, and you "
                "shall place the rings on the four corners that are on its "
                "four legs. The rings shall be opposite the frame as holders "
                "for the poles [with which] to carry the table. And you shall "
                "make the poles of acacia wood, and you shall overlay them "
                "with gold, and the table shall be carried with them. And you "
                "shall make its forms, its spoons, its half pipes, and its "
                "supports with which it will be covered; of pure gold you "
                "shall make them. And you shall place on the table showbread "
                "before Me at all times. "
                "And you shall make a menorah of pure gold. The menorah shall "
                "be made of hammered work; its base and its stem, its goblets, "
                "its knobs, and its flowers shall [all] be [one piece] with "
                "it. And six branches coming out of its sides: three menorah "
                "branches from its one side and three menorah branches from "
                "its second side. Three decorated goblets on one branch, a "
                "knob and a flower, and three decorated goblets on one branch, "
                "a knob and a flower; so for the six branches that come out of "
                "the menorah. And on [the stem of] the menorah [shall be] four "
                "decorated goblets, its knobs and its flowers. And a knob under "
                "the two branches from it, and a knob under the two branches "
                "from it, and a knob under the two branches from it; so for the "
                "six branches that come out of the menorah. Their knobs and "
                "their branches shall [all] be [one piece] with it; all of it "

                "[shall be] one hammered mass of pure gold. And you shall make "
                "its lamps seven, and he shall kindle its lamps [so that they] "
                "shed light toward its face. And its tongs and its scoops "
                "[shall be] of pure gold. He shall make it of a talent of pure "
                "gold, with all these implements. Now see and make according "
                "to their pattern, which you are shown on the mountain. "
                "And the Mishkan you shall make out of ten curtains "
                "[consisting] of twisted fine linen, and blue, purple, and "
                "crimson wool. A cherubim design of the work of a master "
                "weaver you shall make them. The length of one curtain [shall "
                "be] twenty eight cubits, and the width of one curtain [shall "
                "be] four cubits; the same measure for all the curtains. Five "
                "of these curtains shall be joined to one another, and [the "
                "other] five curtains shall [also] be joined to one another. "
                "And you shall make loops of blue wool on the edge of one "
                "curtain [that is] at the edge of the [first] set, and so "
                "shall you do on the edge of the outermost curtain of the "
                "second set. You shall make fifty loops on [the edge of] one "
                "curtain, and you shall make fifty loops on the edge of the "
                "curtain in the second set; the loops shall correspond to one "
                "another. And you shall make fifty golden clasps, and you "
                "shall fasten the curtains to one another with the clasps; so "
                "shall the Mishkan become one. And [then] you shall make "
                "curtains of goat hair for a tent over the Mishkan; you shall "
                "make eleven curtains. The length of one curtain [shall be] "
                "thirty cubits, and the width of one curtain four cubits; the "
                "same measure for the eleven curtains. And you shall join the "
                "five curtains by themselves, and the [other] six curtains by "
                "themselves; and you shall fold the sixth curtain before the "
                "front of the tent. And you shall make fifty loops on the edge "
                "of one curtain, [which is] at the edge of the [first] set, and "
                "fifty loops on the edge of the [outermost] curtain of the "
                "second set. And you shall make fifty copper clasps; you shall "
                "bring the clasps into the loops, and you shall fasten the "
                "tent together so that it will become one. And the overhanging "
                "excess in the curtains of the tent half of the extra curtain "
                "shall hang over the rear of the Mishkan. And the cubit from "
                "here and the cubit from there of the excess in the length of "
                "the curtains of the tent shall hang over the sides of the "
                "Mishkan from here and from there to cover it. And you shall "
                "make a covering for the tent of ram skins dyed red and a "
                "covering of tachash skins above.";
            std::vector<char>::size_type len = strlen(c);
            this->reserve(len);
            this->insert(this->end(), c, c + len);
        }
};


//----------------------------------------------------------------------------//

} } } // End namespaces test, iostreams, boost.

#endif // #ifndef BOOST_IOSTREAMS_TEST_SEQUENCE_HPP_INCLUDED
