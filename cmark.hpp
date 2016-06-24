// cmark.hpp - 2016 - Atlee Brink
// class cmark: Markov factor utensils

#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_set>
#include <string>
#include <utility>
#include <vector>

using namespace std;

template<class Flavor = double, class Color = unsigned int, class Hue = unsigned int>
class
cmark {
    public:

    // a lone box
    struct
    smarkbox {
        vector< pair<Color, size_t> > palette;
        vector<Flavor> flavors;
        Flavor taste( const vector<Hue> &spectrum ) const {
            size_t touch = 0;
            for( const auto &band : palette ) touch += spectrum[ band.first ] * band.second;
            return flavors[ touch ];
        }
    };

    // a lone line of boxes 
    class
    smarkboxline : public unordered_set< shared_ptr< smarkbox > > 
    {};

    // for spectral analysis of boxen
    class
    smarkboxprism : public vector< smarkboxline >
    {};

    static
    shared_ptr<smarkbox>
    merge (
        const vector<Hue> &bandwidths,
        const smarkboxline &line,
        Color color
    ) {
        // new smarkbox
        auto pbox = make_shared<smarkbox>();

        // which hold just one color
        pbox->palette.resize( 1 );
        pbox->palette[0] = { color, 1 };

        // ..and all its hues
        pbox->flavors.resize( bandwidths[color] );

        // what do they taste like?
        for( Hue h = 0; h < bandwidths[color]; h++ ) {
            Flavor bitetaste = (Flavor)1;
            for( auto box : line ) bitetaste *= box->flavors[h];
            pbox->flavors[h] = bitetaste;
        }

        return pbox;
    }

    static
    shared_ptr<smarkbox>
    ostracize (
        const vector<Hue> &bandwidths,
        const smarkboxline &line,
        Color bad
    ) {
        // good colors are adjacent to 'line', which is the bad color
        unordered_set<Color> goodcolors;
        for( const auto pbox : line ) {
            for( const auto pair : pbox->palette ) {
                if( pair.first != bad ) goodcolors.insert( pair.first );
            }
        }

        // new smarkbox
        auto pbox = make_shared<smarkbox>();
        pbox->palette.resize( goodcolors.size() );

        // moves through available hue patterns
        vector< pair<Color, Hue> > pattern( goodcolors.size() );

        // assign a palette to the new smarkbox (colors and strides),
        // and prepare an initial hue pattern.
        size_t i = pbox->palette.size(), width = 1;
        for( auto color : goodcolors ) {
            pbox->palette[i-1] = { color, width };
            width *= bandwidths[ pbox->palette[i-1].first ];
            pattern[i-1] = { color, (Hue)0 };
            i--;
        }

        // result of 'pattern' projected onto full spectrum: changes over time
        vector< Hue > spectrum( bandwidths.size(), (Hue)0 );

        // store flavors of all possible hue patterns from this palette
        for( bool hungry = true; hungry; ) {

            // sum tastes over bad color
            Flavor sumtastes = (Flavor)0;
            for( Hue h = 0; h < bandwidths[bad]; h++ ) {
                spectrum[bad] = h;
                Flavor bitetaste = (Flavor)1;
                for( auto box : line ) bitetaste *= box->taste( spectrum );
                sumtastes += bitetaste;
            }

            pbox->flavors.push_back( sumtastes );

            // adjust pattern and spectrum for next taste
            for( size_t band = pattern.size() - 1;; ) {
                ++pattern[band].second;
                ++spectrum[pattern[band].first];
                if( pattern[band].second < bandwidths[pattern[band].first] ) break;
                if( !band ) {
                    hungry = false;
                    break;
                }
                pattern[band].second = 0;
                spectrum[pattern[band].first] = 0;
                band--;
            }
        }

        return pbox;
    }

    // rounds up boxen
    static
    bool
    wranglesmarks(
        vector< Hue > &bandwidths,
        smarkboxprism &prism,
        const string &markfile
    ) {
        ifstream markflow( markfile );
        if( !markflow ) {
            cerr << "wranglesmarks: markfile \"" << markfile << "\" won't open!\n";
            return false;
        }

        vector< shared_ptr< smarkbox > > smarkorder;

        // preamble
        {
            string type;
            markflow >> type;
            if( type != "MARKOV" ) {
                cerr << "wranglesmarks: markfile \"" << markfile << "\""
                    << " has type \"" << type
                    << "\", but I only understand type \"MARKOV\"!\n";
                return false;
            }

            size_t colorcount;
            markflow >> colorcount;
            bandwidths.resize( colorcount );
            prism.resize( colorcount, smarkboxline() );

            for( auto &width : bandwidths ) {
                size_t bigwidth;
                markflow >> bigwidth;
                width = (Hue)bigwidth;
            }

            size_t prismcard;
            markflow >> prismcard;
            smarkorder.resize( prismcard );

            for( size_t i = 0; i < prismcard; i++ ) {
                auto pbox = make_shared<smarkbox>();

                smarkorder[i] = pbox;

                size_t palettesize;
                markflow >> palettesize;
                pbox->palette.resize( palettesize );

                for( auto &band : pbox->palette ) {
                    size_t color;
                    markflow >> color;
                    if( color >= colorcount ) {
                        cerr << "wranglesmarks: markfile \"" << markfile
                            << " contains an out-of-range variable index!\n";
                        return false;
                    }
                    band.first = (Color)color;
                    prism[color].insert( pbox );
                }

                for(
                    size_t i = pbox->palette.size(), width = 1;
                    i > 0;
                    width *= bandwidths[ pbox->palette[i---1].first ]
                ) {
                    pbox->palette[i-1].second = width;
                }
            }
        }

        // smarkboxen proper
        for( auto &pbox : smarkorder ) {
            size_t volume;
            markflow >> volume;
            pbox->flavors.resize( volume );
            for( auto &flavor : pbox->flavors ) markflow >> flavor;
        }

        if( !markflow.good() ) {
            cerr << "wranglesmarks: markfile \"" << markfile << "\" confused me!\n";
            return false;
        }

        return true;
    }
};

// end
