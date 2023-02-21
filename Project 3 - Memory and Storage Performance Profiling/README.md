# Project 3 - Memory and Storage Performance Profiling
### By: Rojan Karn and Almog Cohen

## Testing Setup for Intel Memory Latency Checker (MLC)

```
mlc --loaded_latency -W2 -l256
```

-r: random accesses

-W2: 2:1 read/write (W6 does 0:1 read/write)

-l: stride size in bytes (-l64 or -l256)

