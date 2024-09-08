#include <cstring>
#include <thread>
#include "lvx.hpp"

template <typename P>
LvxWriter<P>::LvxWriter(std::string path)
{
    _is_device_info_appended = false;
    memset(&_frame_buffer_1, 0, sizeof(LxvFrame<P>));
    memset(&_frame_buffer_2, 0, sizeof(LxvFrame<P>));
    _current_frame = &_frame_buffer_1;
    _current_package_index = 0;

    _current_offset = 0;
    _frame_index = 0;

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
        LvxPublicHeader publicHeader;
        _file.write((const char *)&publicHeader, sizeof(LvxPublicHeader));
        _current_offset += sizeof(LvxPublicHeader);

        LxvPrivateHeader privateHeader;
        privateHeader.device_count = 1;
        _file.write((const char *)&privateHeader, sizeof(LxvPrivateHeader));
        _current_offset += sizeof(LxvPrivateHeader);

        _file.flush();
    }
}
template <typename P>
LvxWriter<P>::~LvxWriter()
{
    if (_file.is_open())
    {
        _file.flush();
        _file.close();
    }
}

template <typename P>
void LvxWriter<P>::appendDeviceInfo(LvxDeviceInfo *lvxDeviceInfo)
{
    if (_file.is_open() && !_is_device_info_appended)
    {
        printf("appendDeviceInfo\n");
        _file.write((const char *)lvxDeviceInfo, sizeof(LvxDeviceInfo));
        _current_offset += sizeof(LvxDeviceInfo);
        _file.flush();

        _is_device_info_appended = true;
    }
}

template <typename P>
void LvxWriter<P>::appendPackageToCurrentFrame(LivoxEthPacket *livoxEthPacket, uint32_t points)
{
    if (_file.is_open() && _is_device_info_appended)
    {
        // auto start_time = std::chrono::steady_clock::now();

        _current_frame->packages[_current_package_index].device_index = 0;
        memcpy(
            (void *)&_current_frame->packages[_current_package_index].version,
            (const void *)livoxEthPacket,
            sizeof(LvxPackage<P>) - 1); /* skip device_index */

        // printf("_current_package_index: %u\n", _current_package_index);
        _current_package_index++;

        // write a completed frame
        if (_current_package_index == PACKAGES_IN_FRAME)
        {
            // there is 50 ms between write requests
            writeCurrentFrame();
        }

        // auto end_time = std::chrono::steady_clock::now();
        // printf("appendPackageToCurrentFrame: %ld ns\n", (end_time - start_time));
    }
}

template <typename P>
void LvxWriter<P>::writeCurrentFrameInBackground(std::ofstream *_file, LxvFrame<P> *_frame)
{
    // auto start_time = std::chrono::steady_clock::now();

    _file->write((char *)_frame, sizeof(LxvFrame<P>));
    _file->flush();

    // auto end_time = std::chrono::steady_clock::now();
    // printf("writeCurrentFrameInBackground: %ld ns\n", (end_time - start_time));
}

template <typename P>
void LvxWriter<P>::writeCurrentFrame()
{
    // auto start_time = std::chrono::steady_clock::now();

    // printf("writeCurrentFrame: %lu\n", _frame_index);

    LxvFrame<P> *_writeback_frame = _current_frame;

    // swap frame buffer
    if (_current_frame == &_frame_buffer_1)
    {
        _current_frame = &_frame_buffer_2;
    }
    else
    {
        _current_frame = &_frame_buffer_1;
    }

    // reset package index
    _current_package_index = 1;

    // update frame header
    _writeback_frame->header.current_offset = _current_offset;
    _writeback_frame->header.next_offset = _current_offset + sizeof(LxvFrame<P>);
    _writeback_frame->header.frame_index = _frame_index;

    // update counters
    _current_offset = _writeback_frame->header.next_offset;
    _frame_index++;

    // write to file in current callback thread
    // due to slow speed, writing to SD Card can take more than 1ms, cause missing ethernet packets
    // _file.write((char *)_writeback_frame, sizeof(LxvFrame<P>));
    // _file.flush();

    // write to file in a new thread in background
    // there is the room of 50 ms for thread to write to file, so it will work on very slow SD Card
    std::thread _write_thread(&LvxWriter::writeCurrentFrameInBackground, this, &_file, _writeback_frame);
    _write_thread.detach(); // let thread run

    // auto end_time = std::chrono::steady_clock::now();
    // printf("writeCurrentFrame: %ld ns\n", (end_time - start_time));
}

template <typename P>
LvxReader<P>::LvxReader(std::string path)
{
    _file.open(path, std::ifstream::binary);
    if (_file.is_open())
    {
        LvxPublicHeader lvxPublicHeader;
        _file.read((char *)&lvxPublicHeader, sizeof(LvxPublicHeader));
        if (std::string((char *)&lvxPublicHeader.signature) == LVX_SIGNATURE &&
            *((uint32_t *)lvxPublicHeader.version) == LVX_VERSION &&
            lvxPublicHeader.magic_code == LVX_MAGIC_CODE)
        {
            printf("Found LVX file!\n");
            LxvPrivateHeader lxvPrivateHeader;
            _file.read((char *)&lxvPrivateHeader, sizeof(LxvPrivateHeader));
            // skip Device info
            _file.seekg(lxvPrivateHeader.device_count * sizeof(LvxDeviceInfo), std::ios_base::cur);
        }
        else
        {
            _file.close();
        }
    }
}

template <typename P>
LvxReader<P>::~LvxReader()
{
    if (_file.is_open())
    {
        _file.close();
    }
}

template <typename P>
LxvFrame<P> *LvxReader<P>::getNextFrame()
{
    if (_file.is_open())
    {
        LxvFrame<P> *lvxFrame = new LxvFrame<P>();
        _file.read((char *)lvxFrame, sizeof(LxvFrame<P>));
        if (_file.fail())
        {
            return nullptr;
        }
        return lvxFrame;
    }

    return nullptr;
}
