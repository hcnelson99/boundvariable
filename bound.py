import sys, tty, termios
fd = sys.stdin.fileno()

f = open("codex.umz", "rb").read()

memory = {}

registers = [0] * 8
finger = 0

memory[0] = []

alloc = 1

for i in range(0, len(f), 4):
    memory[0].append((f[i] << 24) | (f[i+1] << 16) | (f[i+2] << 8) | f[i+3])

print("no ahh")
print(memory[0][0])
print(memory[0][1])
print(memory[0][2])

while True:
    op = (memory[0][finger] >> 28)
    # print("op:", op)
    a = (memory[0][finger] >> 6) & 7
    b = (memory[0][finger] >> 3) & 7
    c = memory[0][finger] & 7
    # print("regs:", a, b, c)
    # print("before", registers)
    if op == 0:
        if registers[c] != 0:
            registers[a] = registers[b]
    elif op == 1:
        registers[a] = memory[registers[b]][registers[c]]
    elif op == 2:
        memory[registers[a]][registers[b]] = registers[c]
    elif op == 3:
        registers[a] = (registers[b] + registers[c]) & 0xFFFFFFFF
    elif op == 4:
        registers[a] = (registers[b] * registers[c]) & 0xFFFFFFFF
    elif op == 5:
        registers[a] = (registers[b] // registers[c]) & 0xFFFFFFFF
    elif op == 6:
        registers[a] = (~(registers[b] & registers[c])) & 0xFFFFFFFF
    elif op == 7:
        break
    elif op == 8:
        new_platter_array = [0] * registers[c]
        # print("alloc", alloc)
        memory[alloc] = new_platter_array
        registers[b] = alloc
        alloc += 1
    elif op == 9:
        del memory[registers[c]]
    elif op == 10:
        sys.stdout.write(chr(registers[c]))
        sys.stdout.flush()
    elif op == 11:
        s = sys.stdin.read(1)
        if s != "":
            registers[c] = ord(s)
        else:
            registers[c] = 0xFFFFFFFF
    elif op == 12:
        if registers[b] != 0:
            memory[0] = list(memory[registers[b]])
        finger = registers[c]-1
    elif op == 13:
        a = (memory[0][finger] >> 25) & 7
        val = memory[0][finger] & 0x1ffffff
        # print("a, val:", a, val)
        registers[a] = val
    else:
        print("Failed to decode op {}  from {}", op, hex(memory[0][finger]))
        break
    finger += 1
    # print("after", registers)
    # print()
