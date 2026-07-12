VORTEX - terminal gateway for open source
-----------------------------------------

Vortex fetches live contributor data from githubcommunity.az
and displays it in your terminal. No bloat. No JS. No tracking.

FEATURES
  vortex                    live dashboard (refreshes every 5h)
  vortex leaderboard        monthly leaderboard
  vortex leaderboard --all  all-time leaderboard
  vortex status <user>      contributor stats
  vortex news               community blog feed

DEPENDENCIES
  gcc, make, curl (runtime only)

BUILD
  make
  make install        # PREFIX=/usr/local

SOURCE
  https://github.com/yourusername/vortex

LICENSE
  GPLv3 - see LICENSE

NOTES
  - All data is fetched live from githubcommunity.az.
    No API key, no registration, no telemetry.
  - The site is public; scraping a public leaderboard
    for a personal CLI tool is considered fair use.
  - Binary size ~30KB. No external C libraries.
  - Suckless philosophy: simple, clean, minimal.
