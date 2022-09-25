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

CPU Frequency: 2997644 kHz
| Function                  | Difference (100 ms) | Time (ms) |
| ------------------------- | ------------------- | --------- |
| clock()                   | 107                 | 413       |
| getChronoSteadyMS()       | 102                 | 471       |
| getChronoSystemMS()       | 108                 | 561       |
| getChronoSteadyNoCastMS() | 107                 | 471       |
| rdtscMS()                 | 108                 | 132       |
| __rdtscMS()               | 101                 | 122       |
| __builtin_ia32_rdtscMS()  | 114                 | 103       |
| clock_gettimeMS()         | 103                 | 298       |
| getSystemTimeMS()         | 106                 | 179       |

## Ubuntu 20 @ WSL (3 GHz)

CPU Frequency: 2994390 kHz
| Function                  | Difference (100 ms) | Time (ms) |
| ------------------------- | ------------------- | --------- |
| clock()                   | 0                   | 1734      |
| getChronoSteadyMS()       | 101                 | 352       |
| getChronoSystemMS()       | 101                 | 355       |
| getChronoSteadyNoCastMS() | 100                 | 288       |
| rdtscMS()                 | 101                 | 98        |
| __rdtscMS()               | 0                   | 20        |
| __builtin_ia32_rdtscMS()  | 100                 | 98        |
| clock_gettimeMS()         | 101                 | 251       |
| getSystemTimeMS()         | 0                   |           |

## Ubuntu 22 @ VM (3 GHz)

CPU Frequency: 3081473 kHz
| Function                  | Difference (100 ms) | Time (ms) |
| ------------------------- | ------------------- | --------- |
| clock()                   | 0                   | 2434      |
| getChronoSteadyMS()       | 100                 | 382       |
| getChronoSystemMS()       | 102                 | 382       |
| getChronoSteadyNoCastMS() | 100                 | 326       |
| rdtscMS()                 | 98                  | 107       |
| __rdtscMS()               | 0                   | 22        |
| __builtin_ia32_rdtscMS()  | 98                  | 101       |
| clock_gettimeMS()         | 100                 | 286       |
| getSystemTimeMS()         | 0                   |           |

## Ubuntu 16 @ Azure (2.59 GHz)

CPU Frequency: 2593872 kHz
| Function                  | Difference (100 ms) | Time (ms) |
| ------------------------- | ------------------- | --------- |
| clock()                   | 0                   | 6103      |
| getChronoSteadyMS()       | 100                 | 466       |
| getChronoSystemMS()       | 100                 | 466       |
| getChronoSteadyNoCastMS() | 100                 | 362       |
| rdtscMS()                 | 100                 | 194       |
| __rdtscMS()               | 0                   | 26        |
| __builtin_ia32_rdtscMS()  | 100                 | 189       |
| clock_gettimeMS()         | 100                 | 313       |
| getSystemTimeMS()         | 0                   |           |

## Raspberry Pi

| Method | Time |
| ------ | ---- |

# Results

CPU Frequency: 2593872 kHz
| Function                  | Speed         | Give time       | Compatible                  |
| ------------------------- | ------------- | --------------- | --------------------------- |
| clock()                   | slow on Linux | Local           | Yes                         |
| getChronoSteadyMS()       | ok            | Local           | Yes                         |
| getChronoSystemMS()       | ok            | Universal       | Yes                         |
| getChronoSteadyNoCastMS() | ok            | Local           | may not work on all systems |
| rdtscMS()                 | fast          | tick            |                             |
| __rdtscMS()               | fast          | tick            | not on Linux                |
| __builtin_ia32_rdtscMS()  | fast          | tick            |                             |
| clock_gettimeMS()         | ok+           | Local/Universal | only on Linux               |
| getSystemTimeMS()         | fast          | Universal       | only on Windows             |

On Windows `clock()` is ok, but is the slowest method on linux.

`getChronoSteady()` is fast and compatible on every devies. `getChronoSteadyNoCast()` is faster, but it may not work on all systems.

The fastest methods are `rdtsc` and `__builtin_ia32_rdtsc` (and `__rdtsc` if available on Linux) but TSC is not safe (if the computer change its frequency).
Also, the TSC may be reset sometimes.

`clock_gettime()` and `getSystemTime()` are fast and doesn't use TSC.
