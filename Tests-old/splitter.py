from tqdm import tqdm
import os
import json
import sys
import time

CB_FILE = "C:\\Users\\fabri\\source\\GBEmu\\test_data\\cb.json"

# load json
with open(CB_FILE, 'r') as f:
    cb = json.load(f)

# loop throu cb with tqdm
stopper = 0
for i in tqdm(cb):
    #time.sleep(0.1)
    # get i->initial->ram->[2]
    opcode = i['initial']['ram'][1][1]
    # remove 0x
    opcode = opcode[2:]
    # if single letter, add 0 before
    if len(opcode) == 1:
        opcode = "0" + opcode

    # save i in list based on opcode
    with open(f"C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB{opcode}.json", 'a') as f:
        f.write(json.dumps(i))
        f.write("\n")