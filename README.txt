┌─────────────────────────────────────────────────────────────┐
│                                                             │
│   V O R T E X                                               │
│   terminal gateway for open source                          │
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

   BUILD (macOS)

     Install Xcode Command Line Tools, then build:

     $ xcode-select --install
     $ make
     $ sudo make install

     curl and make ship with macOS. The system cc (clang) works as gcc.

   BUILD (Windows)

     Option A — WSL (recommended):

       Install WSL (Ubuntu), open the WSL terminal, then:

       $ sudo apt install build-essential curl
       $ make
       $ sudo make install

     Option B — MSYS2 / MinGW:

       1. Install MSYS2 from https://www.msys2.org
       2. Open "MSYS2 MinGW UCRT" terminal
       3. pacman -S mingw-w64-ucrt-x86_64-{gcc,make,curl}
       4. make
       5. make install

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
