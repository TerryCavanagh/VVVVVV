#ifndef CREDITS_H
#define CREDITS_H

#include <SDL.h>

namespace Credits {

/* Terry's Patrons... */
static const char* superpatrons[] = {
    "Anders Ekermo",
    "Andreas K|mper",
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
    "Andreas J{rgensen",
    "}ngel Louzao Penalva",
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
    "Magnus P~lsson",
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

/* CONTRIBUTORS.txt, again listed alphabetically (according to `sort`) by last name */
static const char* githubfriends[] = {
    "Misa \"Info Teddy\" Elizabeth",
    "",
    "and",
    "",
    "Matt \"Fussmatte\" Aaldenberg", // TODO: Change to "Fußmatte" if/when UTF-8 support is added
    "AlexApps99",
    "AllyTally",
    "Christoph B{hmwalder",
    "Charlie Bruce",
    "Brian Callahan",
    "Dav999",
    "Allison Fleischer",
    "Malte Grimm",
    "Daniel Lee",
    "Fredrik Ljungdahl",
    "MAO3J1m0Op",
    "Nichole Mattera",
    "Matt Penny",
    "Tynan Richards",
    "Elliott Saltar",
    "Marvin Scholz",
    "Keith Stellyes",
    "Elijah Stone",
    "Thomas S|nger",
    "Pierre-Alain TORET",
    "leo60228",
    "Emmanuel Vadot",
    "Remi Verschelde", // TODO: Change to "Rémi" if/when UTF-8 support is added
    "viri",
    "Wouter",
};

/* Calculate credits length, finally. */
static const int creditmaxposition = 1050 + (10 * (
    SDL_arraysize(superpatrons) + SDL_arraysize(patrons) + SDL_arraysize(githubfriends)
));

}; /* namespace Credits */

#endif /* CREDITS_H */
