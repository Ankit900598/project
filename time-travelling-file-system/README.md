# Time-Travelling File System

An in-memory, Git-like versioned file system in C++17 (IIT Delhi COL106 assignment).
HashMap, Heap and version trees are written from scratch (no `std::map`, `unordered_map`, `priority_queue`, `set`).

Author: Ankit Yadav (https://github.com/Ankit900598)

## Build and run
```sh
./build.sh                       # g++ -std=c++17 -O2 -Wall -o tffs main.cpp
./tffs < tests/sample_input.txt  # or run ./tffs and type commands
```
Timestamps in `tests/expected_output.txt` are replaced by `<time>`.

## Commands
Content and messages are the rest of the line after the filename.

| Command | Example | Effect |
|---|---|---|
| `CREATE <file>` | `CREATE notes` | New file, version 0 (empty), snapshot "Initial snapshot" |
| `READ <file>` | `READ notes` | Prints content of the active version |
| `INSERT <file> <text>` | `INSERT notes Hello` | Appends text |
| `UPDATE <file> <text>` | `UPDATE notes New text` | Replaces content |
| `SNAPSHOT <file> <msg>` | `SNAPSHOT notes first draft` | Freezes the active version |
| `ROLLBACK <file> [id]` | `ROLLBACK notes 0` / `ROLLBACK notes` | Active = version id, or the parent if no id |
| `HISTORY <file>` | `HISTORY notes` | Snapshots from root to active: `id \| time \| message` |
| `RECENT_FILES [n]` | `RECENT_FILES 2` | Files by last modified time, newest first (default all) |
| `BIGGEST_TREES [n]` | `BIGGEST_TREES 2` | Files by total versions, largest first (default all) |
| `EXIT` | `EXIT` | Stop |

`RECENT FILES` and `BIGGEST TREES` (with a space) also work.

Versioning rule for INSERT/UPDATE: if the active version is a snapshot, a new child version is created
(id = next number for that file) and becomes active; otherwise the active version is edited in place.

## Complexity
N = number of files, k = items requested, d = depth of active version.

| Operation | Time |
|---|---|
| CREATE, READ, SNAPSHOT | O(1) average (hash lookup) |
| INSERT / UPDATE | O(1) average + string copy |
| ROLLBACK id / ROLLBACK | O(1) average |
| HISTORY | O(d) |
| RECENT_FILES / BIGGEST_TREES | O(N + k log N) |

## Design decisions
- **Version tree:** each `TreeNode` keeps its parent and children. Rolling back and editing creates branches.
- **Explicit `is_snapshot` flag** instead of guessing from an empty message.
- **HashMap:** separate chaining (vector of buckets), polynomial string hash (`h = h*31 + c`), ints hash to themselves, doubles buckets when load factor exceeds 0.75. Used for filename -> File and version id -> node.
- **MaxHeap:** array-based binary heap with sift up/down and a comparator. For the two queries a fresh heap is built from all files when the command runs (O(N) heapify) and k items are popped (O(k log N)). This is simpler than keeping a live heap updated on every edit (which needs index tracking / decrease-key) and is fast enough for these queries.
- **Memory:** `TreeNode` destructor deletes its children, `File` deletes its root, `FileSystem` deletes every file.
- Files modified in the same second tie in RECENT_FILES (timestamps have 1-second resolution).

## Error handling
Bad input never stops the program. It prints one line starting with `Error:` and reads the next command. Covered cases:
unknown file, duplicate CREATE, missing filename/content/message, non-numeric or unknown version id,
ROLLBACK at root, SNAPSHOT of an existing snapshot, bad count, unknown command.
