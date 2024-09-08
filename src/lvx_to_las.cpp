#include <fstream>
#include <unistd.h>
#include <memory>

#include "lvx.t.hpp"
#include "las.t.hpp"

int main(int argc, char *argv[])
{
    char *lvx_path = nullptr;
    char *las_path = nullptr;
    char filename[30] = {0};
    int c;
    opterr = 0;

    while ((c = getopt(argc, argv, "i:o:")) != -1)
    {
        switch (c)
        {
        case 'i':
            lvx_path = optarg;
            break;
        case 'o':
            las_path = optarg;
            break;
        default:
            break;
        }
    }

    printf("path: %s\n", lvx_path);
    if (lvx_path == nullptr)
    {
        printf("Use -i lxv\n");
        return -1;
    }

    if (las_path == nullptr)
    {
        time_t curtime = time(nullptr);
        tm *local_time = localtime(&curtime);
        strftime(filename, sizeof(filename), "%Y-%m-%d_%H-%M-%S.las", local_time);
        las_path = filename;
    }

    LvxReader<LivoxRawPoint> lvx_file(lvx_path);
    LasWriter<PointDataType::kCartesian, PointDataFormat0> las_file(las_path);

    uint64_t num_points = 0;

    // while (true)
    // {
        std::unique_ptr<LxvFrame<LivoxRawPoint>> lvx_frame(lvx_file.getNextFrame());
        if (lvx_frame)
        {
            for (int pkg_idx = 0; pkg_idx < PACKAGES_IN_FRAME; pkg_idx++)
            {
                LvxPackage<LivoxRawPoint> *lvx_package = &lvx_frame->packages[pkg_idx];
                for (int pt_idx = 0; pt_idx < POINTS_IN_PACKAGE; pt_idx++)
                {
                    LivoxRawPoint *point = &lvx_package->points[pt_idx];
                    std::unique_ptr<PointDataFormat0> point_data(new PointDataFormat0);
                    memset((void *)point_data.get(), 0, sizeof(PointDataFormat0));
                    point_data->X = point->x;
                    point_data->Y = point->y;
                    point_data->Z = point->z;
                    point_data->intensity = point->reflectivity;
                    las_file.writePointDataRecord(point_data.get());
                    num_points++;
                }
            }
        // }
        // else
        // {
            // break;
        // }
    }

    printf("num_points = %lu\n", num_points);
    return 0;
}