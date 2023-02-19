# Project 3 - Memory and Storage Performance Profiling
### By: Rojan Karn and Almog Cohen

## Testing Setup for Intel Memory Latency Checker (MLC)

```
mlc --loaded_latency –r -W2 -Y
```

-r: random accesses

-W2: 2:1 read/write (W6 does 0:1 read/write)

-Y: AVX2 load and store (256B i think?) - leave out this flag for 64B data access size

