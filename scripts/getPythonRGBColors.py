#!/usr/bin/env python3 

import matplotlib as plt 
import matplotlib.colors as mcolors 

names = list(mcolors.TABLEAU_COLORS)
for c in names:
    r, g, b, a = plt.colors.to_rgba(c)
    print(f"{{{r:.5f}, {g:.5f}, {b:.5f}}}, //{c}")

