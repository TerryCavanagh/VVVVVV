#ifndef CREDITS_H
#define CREDITS_H

#include <SDL.h>

namespace Credits {

/* Terry's Patrons... */
static const char* superpatrons[] = {
    "Anders Ekermo",
    "Andreas Kämper",
    "Anthony Burch",
    "Bennett Foddy",
    "Brendan O'Sullivan",
    "Christopher Armstrong",
    "Daniel Benmergui",
    "David Pittman",
    "Ian Bogost",
    "Ian Poma",
    "Jaz McDougall",
    "John Faulkenbury",
    "Jonathan Whiting",
    "Kyle Pulver",
    "Markus Persson",
    "Nathan Ostgard",
    "Nick Easler",
    "Stephen Lavelle",
};

static const char* patrons[] = {
    "Adam Wendt",
    "Andreas Jörgensen",
    "Ángel Louzao Penalva",
    "Ashley Burton",
    "Aubrey Hesselgren",
    "Bradley Rose",
    "Brendan Urquhart",
    "Chris Ayotte",
    "Christopher Zamanillo",
    "Daniel Schuller",
    "Hybrid Mind Studios",
    "Emilie McGinley",
    "Francisco Solares",
    "Hal Helms",
    "Hayden Scott-Baron",
    "Hermit Games",
    "Ido Yehieli",
    "Jade Vault Games",
    "James Andrews",
    "James Riley",
    "James Hsieh",
    "Jasper Byrne",
    "Jedediah Baker",
    "Jens Bergensten",
    "Jeremy J. Penner",
    "Jeremy Peterson",
    "Jim McGinley",
    "Jonathan Cartwright",
    "John Nesky",
    "Jos Yule",
    "Jose Flores",
    "Josh Bizeau",
    "Joshua Buergel",
    "Joshua Hochner",
    "Kurt Ostfeld",
    "Magnus Pålsson",
    "Mark Neschadimenko",
    "Matt Antonellis",
    "Matthew Reppert",
    "Michael Falkensteiner",
    "Michael Vendittelli",
    "Mike Kasprzak",
    "Mitchel Stein",
    "Sean Murray",
    "Simon Michael",
    "Simon Schmid",
    "Stephen Maxwell",
    "Swing Swing Submarine",
    "Tam Toucan",
    "Terry Dooher",
    "Tim W.",
    "Timothy Bragan",
};

/* CONTRIBUTORS.txt, again listed alphabetically (according to `sort`) by first name
 * Misa is special; she gets to be listed in C++ credits alongside Ethan */
static const char* githubfriends[] = {
    "AlexApps99",
    "Allison Fleischer",
    "AllyTally",
    "Brian Callahan",
    "Charlie Bruce",
    "Christoph Böhmwalder",
    "Daniel Lee",
    "Dav999",
    "Elijah Stone",
    "Elliott Saltar",
    "Emmanuel Vadot",
    "fraZ0R",
    "Fredrik Ljungdahl",
    "Keith Stellyes",
    "KyoZM",
    "leo60228",
    "MAO3J1m0Op",
    "Malte Grimm",
    "Marvin Scholz",
    "Matt Penny",
    "mothbeanie",
    "Nichole Mattera",
    "Pierre-Alain TORET",
    "Reese Rivers",
    "Rémi Verschelde",
    "Thomas Sänger",
    "Tynan Richards",
    "Wouter",
    "viri",
    "Vittorio Romeo",
    "Yussur Mustafa Oraji",
};

/* Calculate credits length, finally. */
static const int creditmaxposition = 1068 + (10 * (
    SDL_arraysize(superpatrons) + SDL_arraysize(patrons) + SDL_arraysize(githubfriends)
));

} /* namespace Credits */

#endif /* CREDITS_H */
