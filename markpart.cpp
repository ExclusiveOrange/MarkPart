// markpart.cpp - 2016 - Atlee Brink
// Markov network partition-function (Z) computer.
// variable-elimination with min-neighbors heuristic, sort of.

#include "cmark.hpp"

namespace {
    typedef double flavor_t;
    typedef unsigned int color_t;
    typedef unsigned int hue_t;

    typedef cmark<flavor_t, color_t, hue_t> mark;
}

int main( int argc, char *argv[] )
{
    // consider user interaction
    if( argc < 2 ) {
        cout << "usage: " << argv[0] << " <infile.uai: markov network file>" << endl;
        return 0;
    }

    // where are the boxen?
    string boxenhabitat( argv[1] );

    // each color may have a different range of hues
    vector< hue_t > bandwidths;

    // smarkboxen should only be viewed through a prism
    mark::smarkboxprism prism;

    // capture boxen
    if( !mark::wranglesmarks( bandwidths, prism, boxenhabitat ) ) {
        cerr << "main: wranglesmarks failed!\n";
        return 1;
    }

    // list colors to eliminate
    unordered_set<color_t> uneliminated( prism.size() );
    for( color_t c = 0; c < prism.size(); c++ ) uneliminated.insert( c );

    // try to eliminate each color once
    while( !uneliminated.empty() ) {

        // find loneliest color (crudely)
        color_t lonely = 0;
        size_t num = 0;
        for( auto c : uneliminated ) {
            if( prism[c].empty() ) continue;
            if( num == 0 ) {
                lonely = c;
                num = prism[c].size();
            }
            else if( prism[c].size() < num ) {
                lonely = c;
                num = prism[c].size();
            }
        }
        if( num == 0 ) {
            cerr << "main: unexpected variable shortage during elimination!" << endl;
            return 1;
        }

        uneliminated.erase( lonely );

        // only ostracize if not already ostracized
        bool hasfriends = false;
        for( auto pbox : prism[lonely] ) {
            if( pbox->palette.size() > 1 ) {
                hasfriends = true;
                break;
            }
        }

        // if no friends, can't ostracize
        if( !hasfriends ) {
            // if more than one with no friends, merge into one
            if( prism[lonely].size() > 1 ) {
                auto merged = mark::merge( bandwidths, prism[lonely], lonely );
                prism[lonely].clear();
                prism[lonely].insert( merged );
            }
            continue;
        }

        // if has friends, ostracize politely and save essence in new smarkbox
        shared_ptr<mark::smarkbox> pnewbox = mark::ostracize( bandwidths, prism[lonely], lonely );

        // finish ostracizing
        while( !prism[lonely].empty() ) {
            auto psmark = *prism[lonely].begin();
            for( auto &set : prism ) set.erase( psmark );
        }

        // cast the new smarkbox into the prism
        for( auto pair : pnewbox->palette ) prism[pair.first].insert( pnewbox );
    }

    // the fast work is done: now remaining colors must be combined slowly
    {
        // find remaining colors
        vector< pair<color_t, hue_t> > pattern;
        for( color_t c = 0; c < prism.size(); c++ ) {
            if( !prism[c].empty() ) pattern.push_back( { c, 0 } );
        }

        // prepare to taste
        flavor_t mealtaste = (flavor_t)0;

        // instructions for tasting
        vector< hue_t > spectrum( bandwidths.size(), (hue_t)0 );

        for( bool hungry = true; hungry; ) {

            // take a bite
            flavor_t bitetaste = (flavor_t)1;
            for( auto &pair : pattern ) {
                bitetaste *= (*prism[pair.first].begin())->taste( spectrum );
            }
            
            mealtaste += bitetaste;

            // adjust spectrum
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

        // output partition function
        cout << "Z = " << mealtaste << endl;
    }

    return 0;
}

// end
