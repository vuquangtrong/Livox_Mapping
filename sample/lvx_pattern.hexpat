/* LVX 1.1.0.0 */

u8 PACKAGES_IN_FRAME = 50;
u8 POINTS_IN_PACKAGE = 100;

struct LivoxRawPoint {
  s32 x;
  s32 y;
  s32 z;
  u8 reflectivity;
};

struct LivoxSpherPoint {
  u32 depth;
  u16 theta;
  u16 phi;
  u8 reflectivity;
};

struct LvxPublicHeader
{
    char signature[16];
    u8 version[4];
    u32 magic_code;
};

struct LxvPrivateHeader
{
    u32 frame_duration;
    u8 device_count;
};

struct LvxDeviceInfo
{
    char lidar_broadcast_code[16];
    char hub_broadcast_code[16];
    u8 device_index;
    u8 device_type;
    u8 extrinsic_enable;
    float roll;
    float pitch;
    float yaw;
    float x;
    float y;
    float z;
};

struct LxvFrameHeader
{
    u64 current_offset;
    u64 next_offset;
    u64 frame_index;
};

struct LvxPackage
{
    u8 device_index;
    u8 version;
    u8 slot_id;
    u8 lidar_index;
    u8 reserved;
    u32 status_code;
    u8 timestamp_type;
    u8 data_type;
    u64 timestamp;
    if (data_type == 0) {
    LivoxRawPoint points[POINTS_IN_PACKAGE];
    } else if (data_type == 1) {
    LivoxSpherPoint points[POINTS_IN_PACKAGE];
    }
};

struct LxvFrame
{
    LxvFrameHeader header;
    LvxPackage packages[PACKAGES_IN_FRAME];
};

struct LvxFile {
    LvxPublicHeader publicHeader;
    LxvPrivateHeader privateHeader;
    LvxDeviceInfo deviceInfos[privateHeader.device_count];
    LxvFrame frames[];
};

LvxFile lvxfile @ 0x00;