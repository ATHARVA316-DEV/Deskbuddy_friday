import sys

colors = {
    '.': "0x0000",
    'R': "IRON_RED",
    'r': "0x9800",
    'G': "IRON_GOLD",
    'g': "0xCC40",
    'C': "HUD_BRIGHT_CYAN",
    'W': "HUD_WHITE",
    'B': "HUD_BLACK",
    'p': "0xF81F", # Pink for hearts
    't': "0x05BF", # Blue tears
    'o': "0xFC00", # Orange thrust
    'y': "0xFFE0"  # Yellow thrust
}

standing = [
    ".......RR.......",
    "......RrrR......",
    ".....RRGGRR.....",
    ".....RWWGRR.....",
    ".....RGgGRR.....",
    ".....RRrrRR.....",
    ".......RR.......",
    ".....RRR.RR.....",
    "....RRRRC.RR....",
    "...RRRrrCrrRR...",
    "..RR.RRrrrr.RR..",
    ".RR..RRRRRR..RR.",
    ".R...RRRRRR...R.",
    "R....RRRRRR....R",
    ".....RRRRRR.....",
    ".....RR..RR.....",
    ".....RR..RR.....",
    ".....R....R.....",
    "....RR....RR....",
    "....RR....RR....",
    "....R......R....",
    "....R......R....",
    "...RR......RR...",
    "...RR......RR..."
]

dancing = [
    ".......RR.......",
    "......RrrR......",
    ".....RRGGRR.....",
    ".....RWWGRR.....",
    ".....RGgGRR.....",
    ".....RRrrRR.....",
    ".......RR.......",
    "...RRRRR.RR.....",
    "..RRRRRRC.RRR...",
    ".RR.RRrrCrr.RR..",
    "RR...RRrrrr..RR.",
    "R....RRRRRR...R.",
    ".....RRRRRR....R",
    ".....RRRRRR.....",
    ".....RRRRRR.....",
    "......RR.RR.....",
    "......RR.RR.....",
    ".......R..R.....",
    "......RR..RR....",
    "......RR..RR....",
    ".......R...R....",
    ".......R...R....",
    "......RR...RR...",
    "......RR...RR..."
]

flying = [
    ".......RR.......",
    "......RrrR......",
    ".....RRGGRR.....",
    ".....RWWGRR.....",
    ".....RGgGRR.....",
    ".....RRrrRR.....",
    ".......RR.......",
    ".....RRR.RR.....",
    "....RRRRC.RR....",
    "...RRRrrCrrRR...",
    "..RR.RRrrrr.RR..",
    ".RR..RRRRRR..RR.",
    ".R...RRRRRR...R.",
    "R....RRRRRR....R",
    ".....RRRRRR.....",
    ".....RR..RR.....",
    ".....RR..RR.....",
    ".....RR..RR.....",
    ".....R....R.....",
    "....RR....RR....",
    "....oo....oo....",
    "...yy......yy...",
    "..yo........oy..",
    "...y........y..."
]

love = [
    "....p.p..p.p....",
    "...ppppp.ppppp..",
    "...ppppp.ppppp..",
    "....ppp...ppp...",
    ".....p.....p....",
    ".......RR.......",
    "......RRGGR.....",
    ".....RppGRp.....",
    ".....RGgGRR.....",
    ".......RR.......",
    ".....RRR.RR.....",
    "....RRRRC.RR....",
    "...RRRrrCrrRR...",
    "..RR.RRrrrr.RR..",
    ".RR..RRRRRR..RR.",
    ".R...RRRRRR...R.",
    "R....RRRRRR....R",
    ".....RRRRRR.....",
    ".....RR..RR.....",
    ".....R....R.....",
    "....RR....RR....",
    "....R......R....",
    "...RR......RR...",
    "...RR......RR..."
]

sad = [
    ".......RR.......",
    "......RrrR......",
    ".....RRGGRR.....",
    ".....RBBGRR.....",
    ".....RtBGRt.....",
    ".....tRrrRt.....",
    ".....t.RR.t.....",
    ".....tRR.RR.....",
    "....RRRRC.RR....",
    "...RRRrrCrrRR...",
    "..RR.RRrrrr.RR..",
    ".RR..RRRRRR..RR.",
    ".R...RRRRRR...R.",
    ".....RRRRRR.....",
    ".....RRRRRR.....",
    ".....RR..RR.....",
    ".....RR..RR.....",
    ".....R....R.....",
    "....RR....RR....",
    "....RR....RR....",
    "....R......R....",
    "....R......R....",
    "...RR......RR...",
    "...RR......RR..."
]

heartbroken = [
    "....p.p.........",
    "...ppppp...p....",
    "...pp.p...ppp...",
    "....p....ppp....",
    "........p.......",
    ".......RR.......",
    "......RrrR......",
    ".....RRGGRR.....",
    ".....RBBGRR.....",
    ".....RGgGRR.....",
    ".....RRrrRR.....",
    ".......RR.......",
    ".....RRR.RR.....",
    "....RRRRC.RR....",
    "...RRRrrCrrRR...",
    "..RR.RRrrrr.RR..",
    ".RR..RRRRRR..RR.",
    ".R...RRRRRR...R.",
    ".....RRRRRR.....",
    ".....RR..RR.....",
    ".....R....R.....",
    "....RR....RR....",
    "...RR......RR...",
    "...RR......RR..."
]

happy = [
    ".......RR.......",
    "......RrrR......",
    ".....RRGGRR.....",
    ".....RyygRR.....",
    ".....RGgGRR.....",
    ".....RRrrRR.....",
    ".......RR.......",
    ".....RRR.RR.....",
    "....RRRRC.RR....",
    "...RRRrrCrrRR...",
    "..RR.RRrrrr.RR..",
    ".RR..RRRRRR..RR.",
    ".R...RRRRRR...R.",
    "RR...RRRRRR...RR",
    ".....RRRRRR.....",
    ".....RR..RR.....",
    ".....RR..RR.....",
    ".....R....R.....",
    "....RR....RR....",
    "....RR....RR....",
    "....R......R....",
    "....R......R....",
    "...RR......RR...",
    "...RR......RR..."
]

shooting = [
    ".......RR.......",
    "......RrrR......",
    ".....RRGGRR.....",
    ".....RWWGRR.....",
    ".....RGgGRR.....",
    ".....RRrrRR.....",
    ".......RR.......",
    ".....RRR.RR.....",
    "....RRRRC.RR....",
    "...RRRrrCrrRRR..",
    "..RR.RRrrrr.RRR.",
    ".RR..RRRRRR..RRR",
    ".R...RRRRRR...CC",
    "R....RRRRRR...CC",
    ".....RRRRRR.....",
    ".....RR..RR.....",
    ".....RR..RR.....",
    ".....R....R.....",
    "....RR....RR....",
    "....RR....RR....",
    "....R......R....",
    "....R......R....",
    "...RR......RR...",
    "...RR......RR..."
]

def gen_array(name, data):
    out = f"const uint16_t {name}[384] PROGMEM = {{\n"
    for row in data:
        out += "    " + ", ".join([colors[c] for c in row]) + ",\n"
    out += "};\n"
    return out

with open("src/display/iron_man_sprites.h", "w") as f:
    f.write("#pragma once\n#include <Arduino.h>\n#include \"ui_theme.h\"\n\n")
    f.write(gen_array("ironManStanding", standing))
    f.write("\n")
    f.write(gen_array("ironManDancing", dancing))
    f.write("\n")
    f.write(gen_array("ironManFlying", flying))
    f.write("\n")
    f.write(gen_array("ironManLove", love))
    f.write("\n")
    f.write(gen_array("ironManSad", sad))
    f.write("\n")
    f.write(gen_array("ironManHeartbroken", heartbroken))
    f.write("\n")
    f.write(gen_array("ironManHappy", happy))
    f.write("\n")
    f.write(gen_array("ironManShooting", shooting))

print("Sprites generated successfully.")
