# Build Problems & Solutions
---

## Problem

Not getting the latest beta version of WiFiManager (latest supports esp32) when installed from PIO.
  
## Solution

Do NOT install from PIO, instead just add following:

     In platformio.ini libs section
        lib_deps = 
        https://github.com/tzapu/WiFiManager.git
---

