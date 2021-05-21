import sys

with open(sys.argv[1], 'r') as f:
    
    race_pairs = set()

    for racepairs in f.readlines():
        racepairs = racepairs[:-1]

        if "@" not in racepairs:
            continue
        
        x, y = racepairs[16:].split('@')
        
        if (x, y) not in race_pairs and (y, x) not in race_pairs:
            print("{}@{}".format(x, y))
            race_pairs.add((x, y))

