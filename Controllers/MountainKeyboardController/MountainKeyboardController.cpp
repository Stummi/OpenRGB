/*------------------------------------------*\
|  MountainKeyboardController.cpp            |
|                                            |
|  Driver for Mountain RGB keyboard lighting |
|  controller                                |
|                                            |
|  Wojciech Lazarski             01/2023     |
|                                            |
\*------------------------------------------*/

#include "MountainKeyboardController.h"
#include <cstring>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

MountainKeyboardController::MountainKeyboardController(hid_device* dev_handle, const char* path)
{
    dev         = dev_handle;
    location    = path;
}

MountainKeyboardController::~MountainKeyboardController()
{
    hid_close(dev);
}

std::string MountainKeyboardController::GetDeviceLocation()
{
    return("HID: " + location);
}

std::string MountainKeyboardController::GetSerialString()
{
    wchar_t serial_string[128];
    int ret = hid_get_serial_number_string(dev, serial_string, 128);

    if(ret != 0)
    {
        return("");
    }

    std::wstring return_wstring = serial_string;
    std::string return_string(return_wstring.begin(), return_wstring.end());

    return(return_string);
}

void MountainKeyboardController::SelectMode(unsigned char mode_idx)
{
    unsigned char usb_buf[MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE];

    memset(usb_buf, 0x00, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
    usb_buf[0x01] = MOUNTAIN_KEYBOARD_SEND_CMD;
    usb_buf[0x02] = MOUNTAIN_KEYBOARD_SELECT_MODE_CMD;
    usb_buf[0x05] = 0x01; //constant data
    usb_buf[0x06] = mode_idx;
    hid_write(dev, usb_buf, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
    std::this_thread::sleep_for(200ms);
}

void MountainKeyboardController::SaveData(unsigned char mode_idx)
{
    unsigned char usb_buf[MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE];

    memset(usb_buf, 0x00, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
    usb_buf[0x01] = MOUNTAIN_KEYBOARD_SAVE_CMD;
    usb_buf[0x02] = MOUNTAIN_KEYBOARD_SAVE_MAGIC1;
    usb_buf[0x05] = mode_idx;
    hid_write(dev, usb_buf, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
    std::this_thread::sleep_for(200ms);
}

void MountainKeyboardController::SendOffCmd()
{
    unsigned char usb_buf[MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE];

    memset(usb_buf, 0x00, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
    usb_buf[0x01] = MOUNTAIN_KEYBOARD_SEND_CMD;
    usb_buf[0x02] = MOUNTAIN_KEYBOARD_SEND_COLOR_DATA_CMD;
    usb_buf[0x03] = MOUNTAIN_KEYBOARD_OFF_MSG;

    usb_buf[0x05] = 0xFF; //constant data
    usb_buf[0x06] = 0x64; // constant data

    usb_buf[0x07] = 0xFF; // constant data
    usb_buf[0x08] = 0xFF; // constant data
    usb_buf[0x09] = 0xFF; // constant data

    hid_write(dev, usb_buf, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
    std::this_thread::sleep_for(10ms);
}


void MountainKeyboardController::SendColorStaticCmd(color_setup setup)
{
    unsigned char usb_buf[MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE];

    memset(usb_buf, 0x00, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
    usb_buf[0x01] = MOUNTAIN_KEYBOARD_SEND_CMD;
    usb_buf[0x02] = MOUNTAIN_KEYBOARD_SEND_COLOR_DATA_CMD;
    usb_buf[0x03] = MOUNTAIN_KEYBOARD_STATIC_MSG;
    usb_buf[0x05] = 0xFF; // constant data
    usb_buf[0x06] = setup.brightness;

    usb_buf[0x07] = 0x00; // constant data
    usb_buf[0x08] = 0xFF; // constant data
    usb_buf[0x09] = 0xFF; // constant data

    usb_buf[0x0A] = setup.mode.one_color.r;
    usb_buf[0x0B] = setup.mode.one_color.g;
    usb_buf[0x0C] = setup.mode.one_color.b;

    hid_write(dev, usb_buf, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
    std::this_thread::sleep_for(10ms);
}

void MountainKeyboardController::SendColorWaveCmd(color_setup setup)
{
    unsigned char usb_buf[MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE];

    memset(usb_buf, 0x00, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
    usb_buf[0x01] = MOUNTAIN_KEYBOARD_SEND_CMD;
    usb_buf[0x02] = MOUNTAIN_KEYBOARD_SEND_COLOR_DATA_CMD;
    usb_buf[0x03] = MOUNTAIN_KEYBOARD_COLOR_WAVE_MSG;

    usb_buf[0x05] = setup.speed;
    usb_buf[0x06] = setup.brightness;
    usb_buf[0x08] = setup.direction;

    switch(setup.color_mode)
    {
        case MOUNTAIN_KEYBOARD_COLOR_MODE_DUAL:
        {
            usb_buf[0x07] = 0x00; // constant data
            usb_buf[0x09] = 0x02; // constant data

            usb_buf[0x0A] = 0x04; // constant data
            usb_buf[0x0B] = 0x19; // constant data

            usb_buf[0x0C] = setup.mode.two_colors.r1;
            usb_buf[0x0D] = setup.mode.two_colors.g1;
            usb_buf[0x0E] = setup.mode.two_colors.b1;
            usb_buf[0x0F] = 0x32; // constant data
            usb_buf[0x10] = setup.mode.two_colors.r2;
            usb_buf[0x11] = setup.mode.two_colors.g2;
            usb_buf[0x12] = setup.mode.two_colors.b2;
            usb_buf[0x13] = 0x4B; // constant data
            usb_buf[0x14] = setup.mode.two_colors.r1;
            usb_buf[0x15] = setup.mode.two_colors.g1;
            usb_buf[0x16] = setup.mode.two_colors.b1;
            usb_buf[0x17] = 0x64; // constant data
            usb_buf[0x18] = setup.mode.two_colors.r2;
            usb_buf[0x19] = setup.mode.two_colors.g2;
            usb_buf[0x1A] = setup.mode.two_colors.b2;
        }
        break;

        case MOUNTAIN_KEYBOARD_COLOR_MODE_SINGLE:
        {
            usb_buf[0x07] = 0x00; // constant data
            usb_buf[0x09] = 0x00; // constant data

            usb_buf[0x0A] = 0x01; // constant data
            usb_buf[0x0B] = 0x64; // constant data

            usb_buf[0x0C] = setup.mode.one_color.r;
            usb_buf[0x0D] = setup.mode.one_color.g;
            usb_buf[0x0E] = setup.mode.one_color.b;
            usb_buf[0x0F] = 0xFF; // constant data
        }
        break;

        case MOUNTAIN_KEYBOARD_COLOR_MODE_RAINBOW:
        {
            usb_buf[0x07] = 0x02; // constant data
            usb_buf[0x09] = 0x02; // constant data

            usb_buf[0x0B] = 0xFF; // constant data
            usb_buf[0x0F] = 0xFF; // constant data
        }
        break;

        default:
        break;
    }

    hid_write(dev, usb_buf, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
    std::this_thread::sleep_for(10ms);
}

void MountainKeyboardController::SendColorTornadoCmd(color_setup setup)
{
    unsigned char usb_buf[MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE];

    memset(usb_buf, 0x00, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
    usb_buf[0x01] = MOUNTAIN_KEYBOARD_SEND_CMD;
    usb_buf[0x02] = MOUNTAIN_KEYBOARD_SEND_COLOR_DATA_CMD;
    usb_buf[0x03] = MOUNTAIN_KEYBOARD_TORNADO_MSG;

    usb_buf[0x05] = setup.speed;
    usb_buf[0x06] = setup.brightness;
    usb_buf[0x08] = setup.direction;

    switch(setup.color_mode)
    {
        case MOUNTAIN_KEYBOARD_COLOR_MODE_SINGLE:
        {
            usb_buf[0x07] = 0x00; // constant data
            usb_buf[0x09] = 0x00; // constant data

            usb_buf[0x0A] = 0x01; // constant data
            usb_buf[0x0B] = 0x64; // constant data

            usb_buf[0x0C] = setup.mode.one_color.r;
            usb_buf[0x0D] = setup.mode.one_color.g;
            usb_buf[0x0E] = setup.mode.one_color.b;
            usb_buf[0x0F] = 0xFF; // constant data
        }
        break;

        case MOUNTAIN_KEYBOARD_COLOR_MODE_RAINBOW:
        {
            usb_buf[0x07] = 0x02; // constant data
            usb_buf[0x09] = 0x02; // constant data

            usb_buf[0x0B] = 0xFF; // constant data
            usb_buf[0x0F] = 0xFF; // constant data
        }
        break;

        default:
        break;
    }
    hid_write(dev, usb_buf, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
    std::this_thread::sleep_for(10ms);
}

void MountainKeyboardController::SendColorBreathingCmd( color_setup setup)
{
    unsigned char usb_buf[MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE];

    memset(usb_buf, 0x00, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
    usb_buf[0x01] = MOUNTAIN_KEYBOARD_SEND_CMD;
    usb_buf[0x02] = MOUNTAIN_KEYBOARD_SEND_COLOR_DATA_CMD;
    usb_buf[0x03] = MOUNTAIN_KEYBOARD_BREATHING_MSG;
    usb_buf[0x05] = setup.speed;
    usb_buf[0x06] = setup.brightness;

    switch(setup.color_mode)
    {
        case MOUNTAIN_KEYBOARD_COLOR_MODE_DUAL:
        {
            usb_buf[0x07] = 0x10; // constant data
            usb_buf[0x08] = 0xFF; // constant data
            usb_buf[0x09] = 0xFF; // constant data

            usb_buf[0x0A] = setup.mode.two_colors.r1;
            usb_buf[0x0B] = setup.mode.two_colors.g1;
            usb_buf[0x0C] = setup.mode.two_colors.b1;
            usb_buf[0x0D] = setup.mode.two_colors.r2;
            usb_buf[0x0E] = setup.mode.two_colors.g2;
            usb_buf[0x0F] = setup.mode.two_colors.b2;
        }
        break;

        case MOUNTAIN_KEYBOARD_COLOR_MODE_SINGLE:
        {
            usb_buf[0x07] = 0x00; // constant data
            usb_buf[0x08] = 0xFF; // constant data
            usb_buf[0x09] = 0xFF; // constant data
            usb_buf[0x0A] = setup.mode.one_color.r;
            usb_buf[0x0B] = setup.mode.one_color.g;
            usb_buf[0x0C] = setup.mode.one_color.b;
        }
        break;

        case MOUNTAIN_KEYBOARD_COLOR_MODE_RAINBOW:
        {
            usb_buf[0x07] = 0x02; // constant data
            usb_buf[0x08] = 0xFF; // constant data
            usb_buf[0x09] = 0xFF; // constant data
        }
        break;

        default:
        break;
    }
    hid_write(dev, usb_buf, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
    std::this_thread::sleep_for(10ms);
}

void MountainKeyboardController::SendColorMatrixCmd(color_setup setup)
{
    unsigned char usb_buf[MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE];

    memset(usb_buf, 0x00, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
    usb_buf[0x01] = MOUNTAIN_KEYBOARD_SEND_CMD;
    usb_buf[0x02] = MOUNTAIN_KEYBOARD_SEND_COLOR_DATA_CMD;
    usb_buf[0x03] = MOUNTAIN_KEYBOARD_MATRIX_MSG;
    usb_buf[0x05] = setup.speed;
    usb_buf[0x06] = setup.brightness;

    usb_buf[0x07] = 0x00; // constant data
    usb_buf[0x08] = 0xFF; // constant data
    usb_buf[0x09] = 0xFF; // constant data

    usb_buf[0x0A] = setup.mode.two_colors.r1;
    usb_buf[0x0B] = setup.mode.two_colors.g1;
    usb_buf[0x0C] = setup.mode.two_colors.b1;

    usb_buf[0x13] = setup.mode.two_colors.r2;
    usb_buf[0x14] = setup.mode.two_colors.g2;
    usb_buf[0x15] = setup.mode.two_colors.b2;

    hid_write(dev, usb_buf, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
    std::this_thread::sleep_for(10ms);
}

void MountainKeyboardController::SendColorReactiveCmd(color_setup setup)
{
    unsigned char usb_buf[MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE];

    memset(usb_buf, 0x00, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
    usb_buf[0x01] = MOUNTAIN_KEYBOARD_SEND_CMD;
    usb_buf[0x02] = MOUNTAIN_KEYBOARD_SEND_COLOR_DATA_CMD;
    usb_buf[0x03] = MOUNTAIN_KEYBOARD_REACTIVE_MSG;
    usb_buf[0x05] = setup.speed;
    usb_buf[0x06] = setup.brightness;

    usb_buf[0x07] = 0x00; // constant data
    usb_buf[0x08] = 0xFF; // constant data
    usb_buf[0x09] = 0xFF; // constant data

    usb_buf[0x0A] = setup.mode.two_colors.r1;
    usb_buf[0x0B] = setup.mode.two_colors.g1;
    usb_buf[0x0C] = setup.mode.two_colors.b1;

    usb_buf[0x13] = setup.mode.two_colors.r2;
    usb_buf[0x14] = setup.mode.two_colors.g2;
    usb_buf[0x15] = setup.mode.two_colors.b2;

    hid_write(dev, usb_buf, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
    std::this_thread::sleep_for(10ms);
}

void MountainKeyboardController::SendColorStartPacketCmd(unsigned char brightness)
{
    unsigned char usb_buf[MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE];

    memset(usb_buf, 0xFF, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
    usb_buf[0x00] = 0x00;
    usb_buf[0x01] = MOUNTAIN_KEYBOARD_SEND_CMD;
    usb_buf[0x02] = MOUNTAIN_KEYBOARD_SEND_COLOR_DATA_CMD;
    usb_buf[0x03] = MOUNTAIN_KEYBOARD_CUSTOM_MSG; // constant data
    usb_buf[0x04] = 0x00; // constant data
    usb_buf[0x06] = brightness;
    hid_write(dev, usb_buf, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
    std::this_thread::sleep_for(10ms);
}

void MountainKeyboardController::SendColorPacketCmd(unsigned char pkt_no,unsigned char brightness, unsigned char *data, unsigned int data_size)
{
    unsigned char usb_buf[MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE];

    memset(usb_buf, 0x00, MOUNTAIN_KEYBOARD_USB_BUFFER_HEADER_SIZE);
    usb_buf[0x01] = MOUNTAIN_KEYBOARD_SEND_CMD;
    usb_buf[0x02] = MOUNTAIN_KEYBOARD_SEND_COLOR_DATA_CMD;
    usb_buf[0x04] = 0x01; // constant data
    usb_buf[0x05] = pkt_no;
    usb_buf[0x06] = brightness;

    if(data_size <= MOUNTAIN_KEYBOARD_USB_MAX_DIRECT_PAYLOAD_SIZE)
    {
        memcpy(&usb_buf[MOUNTAIN_KEYBOARD_USB_BUFFER_HEADER_SIZE],data,data_size);
        if(data_size < MOUNTAIN_KEYBOARD_USB_MAX_DIRECT_PAYLOAD_SIZE)
        {
            memset(&usb_buf[MOUNTAIN_KEYBOARD_USB_BUFFER_HEADER_SIZE + data_size],0x00,MOUNTAIN_KEYBOARD_USB_MAX_DIRECT_PAYLOAD_SIZE-data_size);
        }
        hid_write(dev, usb_buf, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
        std::this_thread::sleep_for(10ms);
    }
}

void MountainKeyboardController::SendColorEdgePacketCmd(unsigned char pkt_no, unsigned char *data, unsigned int data_size)
{
    unsigned char usb_buf[MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE];

    memset(usb_buf, 0x00, MOUNTAIN_KEYBOARD_USB_BUFFER_HEADER_SIZE);
    usb_buf[0x01] = MOUNTAIN_KEYBOARD_SEND_CMD;
    usb_buf[0x02] = MOUNTAIN_KEYBOARD_SEND_COLOR_EDGE_CMD;
    usb_buf[0x03] = 0x0A; // constant data
    usb_buf[0x05] = pkt_no;
    usb_buf[0x06] = 0xFF;

    if(data_size <= MOUNTAIN_KEYBOARD_USB_MAX_DIRECT_PAYLOAD_SIZE)
    {
        memcpy(&usb_buf[MOUNTAIN_KEYBOARD_USB_BUFFER_HEADER_SIZE],data,data_size);
        if(data_size < MOUNTAIN_KEYBOARD_USB_MAX_DIRECT_PAYLOAD_SIZE)
        {
            memset(&usb_buf[MOUNTAIN_KEYBOARD_USB_BUFFER_HEADER_SIZE + data_size],0x00,MOUNTAIN_KEYBOARD_USB_MAX_DIRECT_PAYLOAD_SIZE-data_size);
        }
        hid_write(dev, usb_buf, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
        std::this_thread::sleep_for(10ms);
    }
}

void MountainKeyboardController::SendColorPacketFinishCmd()
{
    unsigned char usb_buf[MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE];

    memset(usb_buf, 0x00, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
    usb_buf[0x01] = MOUNTAIN_KEYBOARD_SEND_CMD;
    usb_buf[0x02] = MOUNTAIN_KEYBOARD_CONFIRM_CMD;

    for(unsigned char i=0;i<3;i++)
    {
        usb_buf[0x03] = i;
        hid_write(dev, usb_buf, MOUNTAIN_KEYBOARD_USB_BUFFER_SIZE);
        std::this_thread::sleep_for(10ms);
    }
}

void MountainKeyboardController::SendDirectColorEdgeCmd(unsigned char brightness, unsigned char *color_data, unsigned int data_size)
{
    unsigned char pkt_no = 0;

    unsigned char *data_ptr = color_data;
    SendColorStartPacketCmd(brightness);
    while(data_size>0)
    {
        if(data_size >= MOUNTAIN_KEYBOARD_USB_MAX_DIRECT_PAYLOAD_SIZE)
        {
            MountainKeyboardController::SendColorEdgePacketCmd(pkt_no++,data_ptr,MOUNTAIN_KEYBOARD_USB_MAX_DIRECT_PAYLOAD_SIZE);
            data_ptr  += MOUNTAIN_KEYBOARD_USB_MAX_DIRECT_PAYLOAD_SIZE;
            data_size -= MOUNTAIN_KEYBOARD_USB_MAX_DIRECT_PAYLOAD_SIZE;
        }
        else
        {
            MountainKeyboardController::SendColorEdgePacketCmd(pkt_no++,data_ptr,data_size);
            data_size = 0;;
        }
    }
    SendColorPacketFinishCmd();
}

void MountainKeyboardController::SendDirectColorCmd(unsigned char brightness, unsigned char *color_data, unsigned int data_size)
{
    unsigned char pkt_no = 0;

    unsigned char *data_ptr = color_data;
    SendColorStartPacketCmd(brightness);
    while(data_size>0)
    {
        if(data_size >= MOUNTAIN_KEYBOARD_USB_MAX_DIRECT_PAYLOAD_SIZE)
        {
            MountainKeyboardController::SendColorPacketCmd(pkt_no++,brightness,data_ptr,MOUNTAIN_KEYBOARD_USB_MAX_DIRECT_PAYLOAD_SIZE);
            data_ptr  += MOUNTAIN_KEYBOARD_USB_MAX_DIRECT_PAYLOAD_SIZE;
            data_size -= MOUNTAIN_KEYBOARD_USB_MAX_DIRECT_PAYLOAD_SIZE;
        }
        else
        {
            MountainKeyboardController::SendColorPacketCmd(pkt_no++,brightness,data_ptr,data_size);
            data_size = 0;;
        }
    }
    SendColorPacketFinishCmd();
}
