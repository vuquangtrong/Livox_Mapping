/* LAS 1.2 */

struct PointDataFormat0
{
    s32 X;
    s32 Y;
    s32 Z;
    u16 intensity;
    u8 attributes;
    u8 classification;
    s8 scan_angle;
    u8 user_data;
    u16 point_source_id;
};

struct LasPublicHeader
{
    char signature[4];
    u16 source_id;
    u16 global_encoding;

    u32 project_id_guid_data_1;
    u16 project_id_guid_data_2;
    u16 project_id_guid_data_3;
    u8 project_id_guid_data_4[8];

    u8 version_major;
    u8 version_minor;

    char system_id[32];
    char generating_software[32];
    u16 file_creation_day_of_year;
    u16 file_creation_year;

    u16 header_size;
    u32 offset_to_point_data;
    u32 number_of_VLRs;
    u8 point_data_format_id;
    u16 point_data_record_length;
    u32 number_of_point_records;
    u32 number_of_point_by_return[5];

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

struct LasFile {
    LasPublicHeader publicHeader;
    PointDataFormat0 pointData @publicHeader.offset_to_point_data;
};

LasFile lasfile @0x00;