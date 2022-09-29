Wich method is the fastest to get the time?

# Methods

- `clock()` returns the time in milliseconds since the program started.
- `getChronoSteady()` uses `std::chrono::steady_clock` to get the time.
- `getChronoSystem()` uses `std::chrono::system_clock` to get the time. It is slower than steady
- `getChronoSteadyNoCast()` uses `std::chrono::steady_clock` to get the time but doesn't use the duration_cast.
- `rdtsc()` uses the `rdtsc` instruction to get the time. Some systems don't support this instruction.
- `__rdtsc()` is not available on linux.
- `__builtin_ia32_rdtsc()` uses the `rdtsc` instruction to get the time. Some systems don't support this instruction.
- `clock_gettime()` uses `clock_gettime` to get the time.

N.B. During the tests, clock() returned a wrong duration with WSL and I don't know why.
The important thing to remember is that clock() is the slowest method, rdtsc is 10 times faster on WSL.

`Difference` calls the method with an interval of 100 ms. If the method doesn't return ~100 ms, it is not working.

`Time` is the time in milliseconds taken by the method to return 10000000 values.
For instance `clock()` with a speed of 1734 ms on WSL is 173 ns / call.

## Goal

The goal is to find the fastest method to get the time on Windows and Linux.

The method must work at least on Windows, WSL and Raspberry Pi.

The time should be in milliseconds and the method should not reset for at least 1 hour.


## Run it on windows

If `rdtsc` is not defined on windows, try to initialize the build with this command:

`cmake -G "MinGW Makefiles" ..`

# Time in ms

## Windows (3 GHz)

CPU Frequency: 2945956 kHz
| Function                          | Difference (100 ms) | Time (ms) | Example       |
| --------------------------------- | ------------------- | --------- | ------------- |
| clock()                           | 108                 | 448       | 557           |
| std::chrono::steady_clock         | 101                 | 641       | 34000982      |
| std::chrono::system_clock         | 114                 | 619       | 1664399420993 |
| std::chrono::steady_clock no cast | 103                 | 473       | 34002295      |
| std::chrono::system_clock no cast | 102                 | 562       | 1664399422238 |
| rdtsc()                           | 114                 | 112       | 34003245      |
| __rdtsc()                         | 109                 | 99        | 34003456      |
| __builtin_ia32_rdtsc()            | 105                 | 99        | 34003663      |
| clock_gettime()                   | 106                 | 293       | 1664399423275 |
| GetSystemTimeAsFileTime()         | 115                 | 154       | 1664399423546 |

## Ubuntu 20 @ WSL (3 GHz)

CPU Frequency: 2994401 kHz
| Function                          | Difference (100 ms) | Time (ms) | Example       |
| --------------------------------- | ------------------- | --------- | ------------- |
| clock()                           | 27                  | 2172      | 2175651       |
| std::chrono::steady_clock         | 100                 | 372       | 8914086       |
| std::chrono::system_clock         | 100                 | 415       | 1664399433974 |
| std::chrono::steady_clock no cast | 101                 | 338       | 8915040       |
| std::chrono::system_clock no cast | 100                 | 446       | 1664399434959 |
| rdtsc()                           | 100                 | 133       | 8915821       |
| __rdtsc()                         | 0                   | 26        | 0             |
| __builtin_ia32_rdtsc()            | 100                 | 118       | 8916165       |
| clock_gettime()                   | 100                 | 265       | 1664399435902 |
| GetSystemTimeAsFileTime()         | 0                   | 23        | 0             |

## Ubuntu 22 @ VM (3 GHz)

CPU Frequency: 3081473 kHz
| Function                          | Difference (100 ms) | Time (ms) |
| --------------------------------- | ------------------- | --------- |
| clock()                           | 0                   | 2434      |
| std::chrono::steady_clock         | 100                 | 382       |
| std::chrono::system_clock         | 102                 | 382       |
| std::chrono::steady_clock no cast | 100                 | 326       |
| rdtscMS()                         | 98                  | 107       |
| __rdtscMS()                       | 0                   | 22        |
| __builtin_ia32_rdtscMS()          | 98                  | 101       |
| clock_gettime()                   | 100                 | 286       |
| GetSystemTimeAsFileTime()         | 0                   |           |

## Ubuntu 16 @ Azure (2.59 GHz)

CPU Frequency: 2593918 kHz
| Function                          | Difference (100 ms) | Time (ms) | Example       |
| --------------------------------- | ------------------- | --------- | ------------- |
| clock()                           | 25                  | 6257      | 6241055       |
| std::chrono::steady_clock         | 100                 | 457       | 167510369     |
| std::chrono::system_clock         | 100                 | 453       | 1664399473579 |
| std::chrono::steady_clock no cast | 100                 | 369       | 167511391     |
| std::chrono::system_clock no cast | 100                 | 358       | 1664399474506 |
| rdtsc()                           | 101                 | 199       | 167512181     |
| __rdtsc()                         | 0                   | 26        | 0             |
| __builtin_ia32_rdtsc()            | 100                 | 188       | 167512596     |
| clock_gettime()                   | 100                 | 327       | 1664399475649 |
| GetSystemTimeAsFileTime()         | 0                   | 27        | 0             |

## Raspberry Pi

| Function                          | Difference (100 ms) | Time (ms) |
| --------------------------------- | ------------------- | --------- |
| clock()                           | 0                   | 52599     |
| std::chrono::steady_clock         | 100                 | 6014      |
| std::chrono::system_clock         | 101                 | 7413      |
| std::chrono::steady_clock no cast | 100                 | 5232      |
| rdtscMS()                         | 0                   | 515       |
| __rdtsc()                         | 0                   | 172       |
| __builtin_ia32_rdtsc()            | 0                   | 174       |
| clock_gettime()                   | 100                 | 1318      |
| GetSystemTimeAsFileTime()         | 0                   | 172       |

# Results

| Function                          | Speed         | Give time       | Compatible                  |
| --------------------------------- | ------------- | --------------- | --------------------------- |
| clock()                           | slow on Linux | Local           | No (!?)                     |
| std::chrono::steady_clock         | ok            | Local           | Yes                         |
| std::chrono::system_clock         | ok            | Universal       | Yes                         |
| std::chrono::steady_clock no cast | ok            | Local           | may not work on all systems |
| std::chrono::system_clock no cast | ok            | Universal       | may not work on all systems |
| rdtsc()                           | fast          | tick            |                             |
| __rdtsc()                         | fast          | tick            | Windows                     |
| __builtin_ia32_rdtsc()            | fast          | tick            |                             |
| clock_gettime()                   | ok+           | Local/Universal |                             |
| GetSystemTimeAsFileTime()         | fast          | Universal       | only on Windows             |

On Windows `clock()` is ok, but is the slowest method on linux.

`std::chrono::steady_clock` is fast and compatible on every devices. `std::chrono::steady_clock no cast` is faster, but it may not work on all systems.
`std::chrono::system_clock` and `std::chrono::system_clock no cast` are similar, but with universal time.

The fastest methods are `rdtsc` and `__builtin_ia32_rdtsc` (and `__rdtsc` if available on Linux) but TSC is not safe (if the computer change its frequency).
Also, the TSC may be reset sometimes.

`clock_gettime()` and `GetSystemTimeAsFileTime()` are fast and doesn't use TSC.
