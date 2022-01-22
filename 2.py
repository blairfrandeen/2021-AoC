dfile = open("data/depths.txt")
depths = [int(line) for line in dfile.readlines()]
# depths = [199,200,208,210,200,207,240,269,260,263]

increases = 0
s_current = sum(depths[0:3])
for i,d in enumerate(depths[:-2]):
    s_next = sum(depths[i+1:i+4])
    if s_next > s_current:
        increases += 1
    s_current = s_next

print(increases)
    
