# Project 3 - Memory and Storage Performance Profiling
### By: Rojan Karn and Almog Cohen

## Testing Setup for Intel Memory Latency Checker (MLC)

```
mlc --loaded_latency –r -W2 -Y
```

-r: random accesses
-W2: 2:1 read/write (W4 does 4:1, etc...)
-Y: AVX2 load and store (256B i think?) - leave out this flag for 64B data access size
