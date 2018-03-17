#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <vector>
#include <string.h>

using namespace std;

int main(int argc, char **argv) {
    FILE* f = fopen(argv[1], "rb");
    assert(f);

    fseek(f, 0, SEEK_END);
    size_t program_size = ftell(f);
    rewind(f);

    assert(program_size % 4 == 0);

    uint32_t *program = (uint32_t*)malloc(program_size + 4);
    program[0] = program_size / 4;
    program++;
    uint32_t finger = 0;
    assert(program);

    for (auto i = 0; i < program_size / 4; i ++) {
        int c1 = fgetc(f);
        int c2 = fgetc(f);
        int c3 = fgetc(f);
        int c4 = fgetc(f);
        program[i] = (c1 << 24) | (c2 << 16) | (c3 << 8) | c4;
    }

    uint32_t reg[8] = {0};

    vector<uint32_t> free_list;
    vector<uint32_t*> memory;
    memory.push_back(program);


    while (true) {
        uint32_t op = memory[0][finger] >> 28;
        uint32_t a = (memory[0][finger] >> 6) & 7;
        uint32_t b = (memory[0][finger] >> 3) & 7;
        uint32_t c = memory[0][finger] & 7;

        if (op == 0) {
            if (reg[c] != 0)
                reg[a] = reg[b];
        } else if (op == 1) {
            reg[a] = memory[reg[b]][reg[c]];
        } else if (op == 2) {
            memory[reg[a]][reg[b]] = reg[c];
        } else if (op == 3) {
            reg[a] = reg[b] + reg[c];
        } else if (op == 4) {
            reg[a] = reg[b] * reg[c];
        } else if (op == 5) {
            reg[a] = reg[b] / reg[c];
        } else if (op == 6) {
            reg[a] = ~(reg[b] & reg[c]);
        } else if (op == 7) {
            break;
        } else if (op == 8) {
            uint32_t *new_array = (uint32_t*)calloc(reg[c] + 1, sizeof(uint32_t));
            new_array[0] = reg[c];
            new_array++;
            if (free_list.size() == 0) {
                memory.push_back(new_array);
                reg[b] = memory.size() - 1;
            } else {
                uint32_t i = free_list.back();
                free_list.pop_back();
                memory[i] = new_array;
                reg[b] = i;
            }
        } else if (op == 9) {
            free(memory[reg[c]] - 1);
            // memory[reg[c]] = nullptr;
            free_list.push_back(reg[c]);
        } else if (op == 10) {
            putchar(reg[c]);
            // fflush(stdout);
        } else if (op == 11) {
            int ch = getchar();
            if (ch != EOF) {
                reg[c] = ch;
            } else {
                reg[c] = 0xFFFFFFFF;
            }
        } else if (op == 12) {
            if (reg[b] != 0) {
               uint32_t len = memory[reg[b]][-1];
               uint32_t *new_array = (uint32_t*)malloc((len + 1)*sizeof(uint32_t));
               new_array[0] = len;
               new_array++;
               memcpy(new_array, memory[reg[b]], len * sizeof(uint32_t));
               free(memory[0] - 1);
               memory[0] = new_array;
            }
            finger = reg[c] - 1;
        } else if (op == 13) {
            a = (memory[0][finger] >> 25) & 7;
            uint32_t val = memory[0][finger] & 0x1ffffff;
            reg[a] = val;
        }
        finger++;
    }
    fclose(f);
}
