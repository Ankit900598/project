# SocialNet Simulator

A small social network backend in C++17 (IIT Delhi COL106 Assignment 2): users and friendships form a graph,
and each user's posts are stored in a hand-written AVL tree.

Author: Ankit Yadav (https://github.com/Ankit900598)

## Build and run
```sh
./build.sh                            # g++ -std=c++17 -O2 -Wall -o socialnet main.cpp
./socialnet < tests/sample_input.txt  # compare with tests/expected_output.txt
```

## Commands
Commands, usernames and post contents are case-insensitive (all lowercased).

| Command | Example | Output |
|---|---|---|
| `ADD USER <u>` | `ADD USER alice` | nothing |
| `ADD FRIEND <u1> <u2>` | `ADD FRIEND alice bob` | nothing (friendship is both ways) |
| `LIST FRIENDS <u>` | `LIST FRIENDS alice` | friends, alphabetical, space-separated |
| `SUGGEST FRIENDS <u> <N>` | `SUGGEST FRIENDS alice 3` | up to N friends-of-friends, most mutual friends first, ties alphabetical; N = 0 prints nothing |
| `DEGREES OF SEPARATION <u1> <u2>` | `DEGREES OF SEPARATION alice dave` | shortest path length, `-1` if not connected |
| `ADD POST <u> "<content>"` | `ADD POST alice "Hello world"` | nothing |
| `OUTPUT POSTS <u> <N>` | `OUTPUT POSTS alice 2` | N newest posts, newest first, one per line; N = -1 for all |

## Complexity
V = users, E = friendships, d = friends of a user, P = posts of a user.

| Operation | Time |
|---|---|
| ADD USER | O(1) average |
| ADD FRIEND | O(d) (duplicate check) |
| LIST FRIENDS | O(d log d) |
| SUGGEST FRIENDS | O(V + sum of friends' degrees + C log C), C = candidates |
| DEGREES OF SEPARATION | O(V + E) (BFS) |
| ADD POST | O(log P) |
| OUTPUT POSTS | O(log P + N) |

## Design
- **Graph:** each user is a vertex number with an adjacency list (`vector<int>`). `unordered_map` maps name -> vertex.
- **AVL tree per user:** key = a global post counter that goes up with every post, so keys are unique and ordered by creation time. Reverse in-order traversal (right, node, left) gives newest first and stops after N.
- **BFS** uses a vector with a moving head index as the queue and a distance array.
- **SUGGEST FRIENDS** counts, for every friend f, each of f's friends that is not the user and not already a friend. That count is the number of mutual friends.
- Destructors free all AVL nodes.

## Error handling
Bad input prints one `Error: ...` line and the program continues: unknown user, duplicate user,
duplicate friendship, self-friendship, wrong number of arguments, non-numeric or negative N, post without quotes, unknown command.
