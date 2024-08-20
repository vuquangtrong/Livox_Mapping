#include <mutex>
#include <condition_variable>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <livox_def.h>
#include <livox_sdk.h>

DeviceInfo lidar_device;
std::mutex lidar_mutex;
std::condition_variable lidar_connected;

/* Callback functin when sample data received */
void OnSampleDataReceived(uint8_t handle, LivoxEthPacket *data, uint32_t data_num, void *client_data)
{
    if (data)
    {
    }
}

/* Callback function when broadcast message received. */
void OnDeviceBroadcastReceived(const BroadcastDeviceInfo *info)
{
    // only connect to the first
    if (info == nullptr || info->dev_type != kDeviceTypeLidarMid40)
    {
        return;
    }
    printf("Receive Broadcast Code: %s,  Device Type: kDeviceTypeLidarMid40\n", info->broadcast_code);

    /* add */
    livox_status ret = AddLidarToConnect(info->broadcast_code, &lidar_device.handle);
    if (ret == kStatusSuccess)
    {
        SetDataCallback(lidar_device.handle, OnSampleDataReceived, nullptr);
        lidar_device.state = kLidarStateUnknown;
    }
}

void OnCommonCommandResonsed(livox_status status, uint8_t handle, uint8_t response, void *client_data)
{
}

void OnLidarConnected(const DeviceInfo *info)
{
    LidarSetMode(info->handle, kLidarModeNormal, OnCommonCommandResonsed, nullptr);
    lidar_connected.notify_one();
}

void OnLidarDisconnected(const DeviceInfo *info)
{
}

void OnLidarStateChanged(const DeviceInfo *info)
{
    switch (info->state)
    {
    case kLidarStateInit:
        printf("OnLidarStateChanged kLidarStateInit progress: %d\n", info->status.progress);
        break;
    case kLidarModeNormal:
        printf("OnLidarStateChanged kLidarModeNormal\n");
        break;
    case kLidarStatePowerSaving:
        printf("OnLidarStateChanged kLidarStatePowerSaving\n");
        break;
    case kLidarStateStandBy:
        printf("OnLidarStateChanged kLidarStateStandBy\n");
        break;
    case kLidarStateError:
        printf("OnLidarStateChanged kLidarStateError error: %x\n", info->status.status_code.error_code);
        break;
    default:
        printf("OnLidarStateChanged state: %d\n", info->state);
        break;
    }
}

/* Callback function when device status changed */
void OnDeviceInfoChanged(const DeviceInfo *info, DeviceEvent event)
{
    if (info == nullptr || info->handle != lidar_device.handle)
    {
        return;
    }

    switch (event)
    {
    case kEventConnect:
        printf("OnDeviceInfoChanged event: kEventConnect\n");
        OnLidarConnected(info);
        break;
    case kEventDisconnect:
        printf("OnDeviceInfoChanged event: kEventDisconnect\n");
        OnLidarDisconnected(info);
        break;
    case kEventStateChange:
        printf("OnDeviceInfoChanged event: kEventStateChange\n");
        OnLidarStateChanged(info);
        break;
    default:
        printf("OnDeviceInfoChanged event: %d\n", event);
        break;
    }
}

void WaitForDevicesReady()
{
    std::unique_lock<std::mutex> lock(lidar_mutex);
    lidar_connected.wait(lock);
}

void ProcessData()
{
    uint8_t delay = 30;
    while (delay)
    {
        sleep(1);
        delay--;
        printf("[%d]\n", delay);
    }
}

void Shutdown()
{
    uint8_t delay = 10;
    while (delay)
    {
        sleep(1);
        delay--;
        printf("[%d]\n", delay);
    }
}


int main(int argc, const char *argv[])
{
    setbuf(stdout, NULL);
    // setvbuf(stdout, NULL, _IONBF, 0);

    printf("Livox Mapping 0.1\n");
    DisableConsoleLogger();

    /* INIT */
    printf("SDK initializing...\n");
    if (!Init())
    {
        return -1;
    }
    printf("SDK initialized.\n");

    LivoxSdkVersion _sdkversion;
    GetLivoxSdkVersion(&_sdkversion);
    printf("SDK version %d.%d.%d\n", _sdkversion.major, _sdkversion.minor, _sdkversion.patch);

    /* REGISTER CALLBACKS */
    SetBroadcastCallback(OnDeviceBroadcastReceived);
    SetDeviceStateUpdateCallback(OnDeviceInfoChanged);
    printf("Callback registered.\n");

    /* START */
    if (!Start())
    {
        goto __exit;
    }
    printf("Discovering device...\n");

    /* WAIT */
    WaitForDevicesReady();

    /* PROCESS DATA */
    ProcessData();

    /* DONE */
    LidarSetMode(lidar_device.handle, kLidarModePowerSaving, OnCommonCommandResonsed, nullptr);

    /* EXIT */
__exit:
    printf("Shuting down...\n");
    Shutdown();
    Uninit();
    printf("Livox SDK UNinitialized.\n");
    return 0;
}