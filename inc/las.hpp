/** LAS Specification Version 1.2 */

#ifndef LAS_HEADER
#define LAS_HEADER

#include <fstream>
#include <stdint.h>

#define LAS_SIGNATURE "LASF"
#define LAS_VERSION_MAJOR 1
#define LAS_VERSION_MINOR 2

#pragma pack(1)

// do not use short, int, long type because
// sizeof them may have different size with defined in LAS spec

struct PointDataFormat0
{
    int32_t X;
    int32_t Y;
    int32_t Z;
    uint16_t intensity;
    uint8_t attributes;
    uint8_t classification;
    char scan_angle;
    uint8_t user_data;
    uint16_t point_source_id;
};

struct LasPublicHeader
{
    char signature[4];
    uint16_t source_id;
    uint16_t global_encoding;

    uint32_t project_id_guid_data_1;
    uint16_t project_id_guid_data_2;
    uint16_t project_id_guid_data_3;
    uint8_t project_id_guid_data_4[8];

    uint8_t version_major;
    uint8_t version_minor;

    char system_id[32];
    char generating_software[32];
    uint16_t file_creation_day_of_year;
    uint16_t file_creation_year;

    uint16_t header_size;
    uint32_t offset_to_point_data;
    uint32_t number_of_VLRs;
    uint8_t point_data_format_id;
    uint16_t point_data_record_length;
    uint32_t number_of_point_records;
    uint32_t number_of_point_by_return[5];

    double x_scale_factor;
    double y_scale_factor;
    double z_scale_factor;
    double x_offset;
    double y_offset;
    double z_offset;
    double max_x;
    double min_x;
    double max_y;
    double min_y;
    double max_z;
    double min_z;
};

#pragma pack()

template <PointDataType T, typename P>
class LasWriter
{
public:
    LasWriter(std::string path = "");
    ~LasWriter();

    void writePointDataRecord(P *pointDataRecord);

private:
    std::ofstream _file;
    LasPublicHeader _publicHeader;
};

#endif
