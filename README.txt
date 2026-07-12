┌─────────────────────────────────────────────────────────────┐
│                                                             │
│   V O R T E X                                              │
│   terminal gateway for open source                         │
│                                                             │
│   Live contributor data from githubcommunity.az             │
│   right in your terminal. No bloat. No JS. No tracking.     │
│                                                             │
└─────────────────────────────────────────────────────────────┘

  COMMANDS

    vortex                     live dashboard (5h refresh)
    vortex leaderboard         monthly leaderboard
    vortex leaderboard --all   all-time leaderboard
    vortex status <user>       contributor stats
    vortex news                community blog feed
    vortex help                this message

  DEPENDENCIES

    gcc, make, curl (runtime only)

  BUILD

    $ make
    $ sudo make install          # PREFIX=/usr/local

  DATA SOURCE

    https://githubcommunity.az

    All data is fetched live. No API key needed.
    No registration. No telemetry.

  BINARY SIZE

    ~30KB. Zero external C libraries.

  LICENSE

    GNU General Public License v3 — see LICENSE

  PHILOSOPHY

    Suckless. Simple, clean, minimal. Code you can read.
