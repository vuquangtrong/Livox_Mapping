#include <string.h>
#include "las.hpp"

template <PointDataType T, typename P>
LasWriter<T, P>::LasWriter(std::string path)
{
    memset((void *)&_publicHeader, 0, sizeof(LasPublicHeader));
    memcpy(&_publicHeader.signature, LAS_SIGNATURE, strlen(LAS_SIGNATURE));
    _publicHeader.version_major = LAS_VERSION_MAJOR;
    _publicHeader.version_minor = LAS_VERSION_MINOR;
    _publicHeader.header_size = sizeof(LasPublicHeader);
    _publicHeader.offset_to_point_data = sizeof(LasPublicHeader);
    _publicHeader.number_of_VLRs = 0;
    _publicHeader.point_data_format_id = T;
    _publicHeader.point_data_record_length = sizeof(P);
    _publicHeader.number_of_point_records = 0;
    _publicHeader.x_scale_factor = 1.0f;
    _publicHeader.y_scale_factor = 1.0f;
    _publicHeader.z_scale_factor = 1.0f;

    if (path == "")
    {
        time_t curtime = time(nullptr);
        char filename[30] = {0};
        tm *local_time = localtime(&curtime);
        strftime(filename, sizeof(filename), "%Y-%m-%d_%H-%M-%S.lvx", local_time);

        path = std::string(filename);
    }

    _file.open(path, std::ios::out | std::ios::binary);
    if (_file.is_open())
    {
        printf("Init LAS file\n");

        _file.write((const char *)&_publicHeader, sizeof(LasPublicHeader));
        _file.flush();
    }
}

template <PointDataType T, typename P>
LasWriter<T, P>::~LasWriter()
{
    if (_file.is_open())
    {
        _file.flush();
        _file.close();
    }
}
template <PointDataType T, typename P>
void LasWriter<T, P>::writePointDataRecord(P *pointDataRecord)
{
    // write point data
    _file.seekp(0, std::ostream::end);
    _file.write((char *)pointDataRecord, sizeof(P));

    // update header
    _publicHeader.number_of_point_records++;

    if (_publicHeader.max_x < (double)pointDataRecord->X)
    {
        _publicHeader.max_x = (double)pointDataRecord->X;
    }
    if (_publicHeader.max_y < (double)pointDataRecord->Y)
    {
        _publicHeader.max_y = (double)pointDataRecord->Y;
    }
    if (_publicHeader.max_z < (double)pointDataRecord->Z)
    {
        _publicHeader.max_z = (double)pointDataRecord->Z;
    }

    if (_publicHeader.min_x > (double)pointDataRecord->X)
    {
        _publicHeader.min_x = (double)pointDataRecord->X;
    }
    if (_publicHeader.min_y > (double)pointDataRecord->Y)
    {
        _publicHeader.min_y = (double)pointDataRecord->Y;
    }
    if (_publicHeader.min_z > (double)pointDataRecord->Z)
    {
        _publicHeader.min_z = (double)pointDataRecord->Z;
    }
    _file.seekp(0, std::ostream::beg);
    _file.write((char *)&_publicHeader, sizeof(LasPublicHeader));
    _file.flush();
}
