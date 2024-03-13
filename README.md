# osu-parsers

A small parsing library designed for my personal projects.

* osr (decoder | ~encoder~)

* osu (~decoder~ | ~encoder~)

* db  (~decoder~ | ~encoder~)

### Example Usage

```cpp
#include <osp/osr/replay_file.h>

int main() {
    const auto replay = osp::osr::from_file("test.osr");

    if(replay == nullptr) {
      // Handle invalid case...
      return EXIT_FAILURE;
    }

    // Do something...

    return EXIT_SUCCESS;
}
```

[Example Project](https://github.com/pushfq/replay_player)

### Building

osu-parsers requires a C++23-compatible compiler as well as LibLZMA.

```cmake
target_link_libraries(my_project PRIVATE osu-parsers::osu-parsers)
```
