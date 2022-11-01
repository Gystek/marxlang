Marxlang
========
Marxlang is a an esoteric language that is impossible for a human
being to program by hand with it. \\

The name come from the fact that the dictionnary is the *Communist
Manifesto* by Marx and Engels, although this text is perfectly
replaceable by any book.

Specification
-------------
Each instruction is a word that MUST BE present in the
dictionnary. Case is ignored.

If a word is present more than one time in the dictionnary, its index
"cycles" through the occurences.

The opcode corresponding to a word is calculated using the following
equation: `I=(((i & 1 && x >> 8 || x) ^ ((i >> 8) + i)) + (p & ((i & 1 && x >> 8 || x) ^ ((i >> 8) + i)))) % 112`
where:
- `i` is the number of non-`NOP` instructions compiled so far (no
counting the current one).
- `x` is the index of the word in the dictionnary.
- `p` is the value of the previous opcode (including `NOP`s this
time). For the first opcode, `p` is `0`.

The processor has 3 registers: `A`, `B` and `flippin`.

The interpreter processes the instruction as follows:

| I  | PC                | Action                                    |
|----|-------------------|-------------------------------------------|
| 3  |         /         | Copy the value from `flippin` to `a`      |
| 1  |         /         | Copy the value from `flippin` to `b`      |
| 4  |         /         | Copy the value from `a` to `flippin`      |
| 15 |         /         | Copy the value from `b` to `flippin`      |
| 9  |         /         | Jump to `a << 8 \| b` if `flippin` ≠ 0    |
|----|-------------------|-------------------------------------------|
| 2  |         3k        | Set `flippin` to `a ^ b`                  |
| 2  |        3k+1       | Set `flippin` to `a \| b`                 |
| 2  |        3k+2       | Set `flippin` to `~(a & b)`               |
|----|-------------------|-------------------------------------------|
| 6  | 6k+a, a ∈ {1,3,5} | Rotate `a` one bit left                   |
| 6  |  6k+a, a ∈ {2,4}  | Rotate `a` one bit right                  |
|----|-------------------|-------------------------------------------|
| 5  |         /         | Output the value of `flippin` in ASCII    |
| 35 |         /         | Input an ASCII character into `flippin`   |
| 8  |         /         | Stop the execution of the virtual machine |

Challenges
----------
I'm not gonna do them, but please email me if you do.

- [ ] Write a "Hello, World!" program.
- [ ] Write a syntaxically correct program.
- [ ] Write a program with actual sense.

Licence
-------
This program is licensed under the Beer-Ware License. As long as you
retain the license notice in the file you can do whatever you want
with this stuff. If we meet some day, and you think this stuff is
worth it, you can buy me a beer in return.