# LVX File

Normally, Lidar sends 1000 ethernet packets every 1 ms. Each packet has 100 points.
The size of packet data is 1318 bytes (header = 18 bytes, data = 100 * 13 = 1300 bytes)

LVX contains frames, each frame is set at fixed duration of 50 ms.
A frame contains multiple packages which is usually the ethernet packet sent from Lidar.

For Mid-40:
100000 pts/s = 20 frm/s x 50 pkg/frm x 100 pts/pkg

## LVX File Format

``` json
{
    "public_header_block": {
        "signature": "livox_tech",
        "version_A": 1, 
        "version_B": 1, 
        "version_C": 0, 
        "version_D": 0,
        "magic_code": 0xAC0EA767
    },
    "private_header_block" : {
        "frame_duration": 50, /* ms */
        "device_count" : N
    },
    "device_info_block": [
        "device_info_1": {...},
        ...,
        "device_info_N": {...}
    ],
    "point_data_block": [
        "frame_1": {
            "frame_header": {
                "current_offset",
                "next_offset",
                "frame_index"
            },
            "package_1": {
                "device_index",
                "version": 5,
                "slot_id",
                "lidar_id": 1, /* Mid-40 */
                "reserved",
                "status_code",
                "timestamp_type": 1, /* PTP */
                "data_type": 0, /* cartesian coord., 100 points/package */
                "timestamp", /* ns */
                "point_1": { /* LivoxRawPoint */
                    "x",
                    "y",
                    "z",
                    "reflectivity"
                }
                ...,
                "point_100": {...}
            },
            ...,
            "package_50": {...}
        },
        ...,
        "frame_M": {...}
    ]
}
```

# Performance

Write directly to SD Card in the callback thread:

```
/* normal packages */
appendPackageToCurrentFrame: 1614 ns

/* last package which trigger writting to frame */
writeCurrentFrame: 1633594 ns
appendPackageToCurrentFrame: 1714375 ns 
```

Write to SD Card in a new thread:

```
/* normal packages */
appendPackageToCurrentFrame: 1666 ns

/* last package which trigger writting to frame */
writeCurrentFrame: 82031 ns
appendPackageToCurrentFrame: 143489 ns

/* write thread */
writeCurrentFrameInBackground: 1134375 ns
```


# Debug

Run in GDB:

```
gdb ./livox_mapping
```

Check Memory Leaks:

```
valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./livox_mapping
```
>
```
==14509== 
==14509== HEAP SUMMARY:
==14509==     in use at exit: 0 bytes in 0 blocks
==14509==   total heap usage: 3,728 allocs, 3,728 frees, 280,838 bytes allocated
==14509== 
==14509== All heap blocks were freed -- no leaks are possible
==14509== 
==14509== For counts of detected and suppressed errors, rerun with: -v
==14509== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```



