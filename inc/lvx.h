/* LVX Specifications v1.1.0.0 */

#ifndef LVX_HEADER
#define LVX_HEADER

#include <fstream>

#include <stdint.h>
#include <livox_def.h>

#define PACKAGES_IN_FRAME 50
#define POINTS_IN_PACKAGE 100

#pragma pack(1)

typedef struct
{
    uint8_t signature[16]{'l', 'i', 'v', 'o', 'x', '_', 't', 'e', 'c', 'h'};
    uint8_t version[4]{1, 1, 0, 0};
    uint32_t magic_code{0xAC0EA767};
} LvxPublicHeader;

typedef struct
{
    uint32_t frame_duration{50};
    uint8_t device_count;
} LxvPrivateHeader;

typedef struct
{
    uint8_t lidar_broadcast_code[16];
    uint8_t hub_broadcast_code[16];
    uint8_t device_index;
    uint8_t device_type;
    uint8_t extrinsic_enable;
    float roll;
    float pitch;
    float yaw;
    float x;
    float y;
    float z;
} LvxDeviceInfo;

typedef struct
{
    uint64_t current_offset;
    uint64_t next_offset;
    uint64_t frame_index;
} LxvFrameHeader;

typedef struct
{
    uint8_t device_index;
    uint8_t version{5}; /* fixed in specs */
    uint8_t slot_id;
    uint8_t lidar_index;
    uint8_t reserved;
    uint32_t status_code;
    uint8_t timestamp_type;
    uint8_t data_type;
    uint64_t timestamp;
    LivoxSpherPoint points[POINTS_IN_PACKAGE];
} LvxPackage;

typedef struct
{
    LxvFrameHeader header;
    LvxPackage packages[PACKAGES_IN_FRAME];
} LxvFrame;

#pragma pack()

class LvxWriter
{
public:
    LvxWriter();
    ~LvxWriter();

    void appendDeviceInfo(LvxDeviceInfo *lvxDeviceInfo);
    void appendPackageToCurrentFrame(LivoxEthPacket *livoxEthPacket, uint32_t points);

private:
    void writeCurrentFrameInBackground(std::ofstream *_file, LxvFrame *_frame);
    void writeCurrentFrame();

    std::ofstream _file;
    bool _is_device_info_appended;
    LxvFrame _frame_buffer_1;
    LxvFrame _frame_buffer_2;
    LxvFrame *_current_frame;
    uint8_t _current_package_index;

    uint64_t _current_offset;
    uint64_t _frame_index;
};

#endif
