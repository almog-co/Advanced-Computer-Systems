# Project 3 - Memory and Storage Performance Profiling
### By: Rojan Karn and Almog Cohen

## Testing Setup for Intel Memory Latency Checker (MLC)

```
mlc --loaded_latency -W2 -l256
```

-r: random accesses

-W2: 2:1 read/write (W6 does 0:1 read/write)

-l: stride size in bytes (-l64 or -l256)

## Testing setup for Flexible I/O (FIO)
###Read-Only
```
fio --name=read_test --ioengine=libaio --iodepth=64 --rw=read --bs=4k --direct=1 --size=1G --numjobs=1 --runtime=60 --time_based --group_reporting --filename=/dev/sda5 --output=/home/acohen/Desktop/FIO_RESULTS/read_only_4k.txt
```

###Write-Only
```
fio --name=write_test --ioengine=libaio --iodepth=64 --rw=write --bs=4k --direct=1 --size=1G --numjobs=1 --runtime=60 --time_based --group_reporting --filename=/dev/sda5 --output=/home/acohen/Desktop/FIO_RESULTS/write_only_4k.txt
```

###Read-Write 70:30
```
fio --name=randrw_test --ioengine=libaio --iodepth=64 --rw=randrw --rwmixread=70 --bs=128k --direct=1 --size=1G --numjobs=1 --runtime=60 --time_based --group_reporting --filename=/dev/sda5 --output=/home/acohen/Desktop/FIO_RESULTS/read_write_70_30_128k.txt
```
