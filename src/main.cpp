#include <mutex>
#include <condition_variable>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <livox_def.h>
#include <livox_sdk.h>
#include "lvx.h"

uint8_t lidar_handle;
std::mutex lidar_mutex;
std::condition_variable lidar_connected;
std::condition_variable lidar_sampling_stopped;
LvxFile lvxFile;

void OnDeviceBroadcastReceived(const BroadcastDeviceInfo *info)
{
    // only connect to Mid-40 lidar
    if (info == nullptr || info->dev_type != kDeviceTypeLidarMid40)
    {
        return;
    }

    printf("Receive Broadcast Code: %s\n", info->broadcast_code);
    livox_status ret = AddLidarToConnect(info->broadcast_code, &lidar_handle);
    if (ret == kStatusSuccess)
    {
        printf("Assigned to handle: %u\n", lidar_handle);
        LvxDeviceInfo lvxDeviceInfo;
        memset((void *)&lvxDeviceInfo, 0, sizeof(LvxDeviceInfo));
        memcpy(
            (void *)&lvxDeviceInfo.lidar_broadcast_code,
            (const void *)info->broadcast_code,
            16);
        lvxDeviceInfo.device_type = kDeviceTypeLidarMid40;
        lvxFile.appendDeviceInfo(&lvxDeviceInfo);
    }
}

void OnSampleDataReceived(uint8_t handle, LivoxEthPacket *eth_packet, uint32_t points, void *client_data)
{
    // normally, lidar sends a new packet every 1 ms
    lvxFile.appendPackageToCurrentFrame(eth_packet, points);
}

void OnLidarErrorReceived(livox_status status, uint8_t handle, ErrorMessage *message)
{
    printf("=== ERROR ===\n");
    printf("temp_status: %u\n", message->lidar_error_code.temp_status);
    printf("volt_status: %u\n", message->lidar_error_code.volt_status);
    printf("motor_status: %u\n", message->lidar_error_code.motor_status);
    printf("dirty_warn: %u\n", message->lidar_error_code.dirty_warn);
    printf("firmware_err: %u\n", message->lidar_error_code.firmware_err);
    printf("pps_status: %u\n", message->lidar_error_code.device_status);
    printf("fan_status: %u\n", message->lidar_error_code.fan_status);
    printf("self_heating: %u\n", message->lidar_error_code.self_heating);
    printf("ptp_status: %u\n", message->lidar_error_code.ptp_status);
    printf("time_sync_status: %u\n", message->lidar_error_code.time_sync_status);
    printf("system_status: %u\n", message->lidar_error_code.system_status);
    printf("=============\n");
}

void OnLidarConnected(const DeviceInfo *info)
{
    SetErrorMessageCallback(info->handle, OnLidarErrorReceived);
    printf("Request lidar power on\n");
    LidarSetMode(info->handle, kLidarModeNormal, nullptr, nullptr);
    lidar_connected.notify_all();
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
        SetSphericalCoordinate(lidar_handle, nullptr, nullptr);
        SetDataCallback(lidar_handle, OnSampleDataReceived, nullptr);
        printf("Start sampling...\n");
        LidarStartSampling(lidar_handle, nullptr, nullptr);
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

void OnDeviceInfoChanged(const DeviceInfo *info, DeviceEvent event)
{
    // only process registered Mid-40 lidar
    if (info == nullptr || info->handle != lidar_handle)
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
    std::unique_lock<std::mutex> lock(lidar_mutex);
    lidar_sampling_stopped.wait(lock);
}

void PrepareShutdown()
{
    printf("Request LVX close\n");
    lvxFile.close();

    printf("Request lidar power off\n");
    LidarSetMode(lidar_handle, kLidarModePowerSaving, nullptr, nullptr);

    sleep(10);
}

void ctr_c_handler(int s)
{
    printf("Caught signal Ctrl+c\n");
    lidar_connected.notify_all();
    lidar_sampling_stopped.notify_all();
}

int main(int argc, const char *argv[])
{
    /* OPTIONS */
    // setbuf(stdout, NULL);
    // setvbuf(stdout, NULL, _IONBF, 0);

    /* EXCEPTION */
    struct sigaction _sigaction;
    _sigaction.sa_handler = ctr_c_handler;
    sigemptyset(&_sigaction.sa_mask);
    _sigaction.sa_flags = 0;
    sigaction(SIGINT, &_sigaction, NULL);

    /* MAIN */
    printf("Livox Mapping 0.1\n");

    /* INIT */
    printf("SDK initializing...\n");
    DisableConsoleLogger();
    if (!Init())
    {
        return -1;
    }
    printf("SDK initialized.\n");

    LivoxSdkVersion _sdkversion;
    GetLivoxSdkVersion(&_sdkversion);
    printf("SDK version %d.%d.%d\n", _sdkversion.major, _sdkversion.minor, _sdkversion.patch);

    printf("LVX initializing...\n");
    if (!lvxFile.init())
    {
        goto __exit;
    }
    printf("LVX initialized.\n");

    /* REGISTER GLOBAL CALLBACKS */
    SetBroadcastCallback(OnDeviceBroadcastReceived);
    SetDeviceStateUpdateCallback(OnDeviceInfoChanged);
    printf("Global Callbacks registered.\n");

    /* START */
    if (!Start())
    {
        goto __exit;
    }
    printf("Discovering device...\n");

    /* WAIT */
    WaitForDevicesReady();
    printf("Device connected\n");

    /* PROCESS DATA */
    printf("Start processing...\n");
    ProcessData();

    /* WORK DONE */
    printf("Preparing for shutdown...\n");
    PrepareShutdown();

    /* EXIT */
__exit:
    printf("Shutting down...\n");
    Uninit();
    printf("Livox SDK UNinitialized.\n");
    return 0;
}