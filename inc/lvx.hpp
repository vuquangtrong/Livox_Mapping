/* LVX Specifications v1.1.0.0 */

#ifndef LVX_HEADER
#define LVX_HEADER

#include <fstream>

#include <stdint.h>
#include <livox_def.h>

#define LVX_SIGNATURE "livox_tech"
#define LVX_VERSION 0x00000101
#define LVX_MAGIC_CODE 0xAC0EA767

#define PACKAGES_IN_FRAME 50
#define POINTS_IN_PACKAGE 100

#pragma pack(1)

struct LvxPublicHeader
{
    uint8_t signature[16];
    uint8_t version[4];
    uint32_t magic_code;
};

struct LxvPrivateHeader
{
    uint32_t frame_duration;
    uint8_t device_count;
};

struct LvxDeviceInfo
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
};

struct LxvFrameHeader
{
    uint64_t current_offset;
    uint64_t next_offset;
    uint64_t frame_index;
};

template <typename P>
struct LvxPackage
{
    uint8_t device_index;
    uint8_t version;
    uint8_t slot_id;
    uint8_t lidar_index;
    uint8_t reserved;
    uint32_t status_code;
    uint8_t timestamp_type;
    uint8_t data_type;
    uint64_t timestamp;
    P points[POINTS_IN_PACKAGE];
};

template <typename P>
struct LxvFrame
{
    LxvFrameHeader header;
    LvxPackage<P> packages[PACKAGES_IN_FRAME];
};

#pragma pack()

template <typename P>
class  LvxWriter
{
public:
    LvxWriter(std::string path = "");
    ~LvxWriter();

    void appendDeviceInfo(LvxDeviceInfo *lvxDeviceInfo);
    void appendPackageToCurrentFrame(LivoxEthPacket *livoxEthPacket, uint32_t points);

private:
    void writeCurrentFrameInBackground(std::ofstream *_file, LxvFrame<P> *_frame);
    void writeCurrentFrame();

    std::ofstream _file;
    bool _is_device_info_appended;
    LxvFrame<P> _frame_buffer_1;
    LxvFrame<P> _frame_buffer_2;
    LxvFrame<P> *_current_frame;
    uint8_t _current_package_index;

    uint64_t _current_offset;
    uint64_t _frame_index;
};

template <typename P>
class LvxReader
{
public:
    LvxReader(std::string path);
    ~LvxReader();

    LxvFrame<P> *getNextFrame();

private:
    std::ifstream _file;
};

#endif
