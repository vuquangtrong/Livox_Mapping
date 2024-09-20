# Livox_Mapping
Lidar Mapping using Lidar, GPS and IMU

To build project:

``` bash
cmake -B build .
```

``` bash
make -C build -j $(nproc)
```

Run binaries:

``` bash
build/lvx_recorder
```

``` bash
build/lvx_to_las
```
