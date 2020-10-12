/*-----------------------------------------*\
|  MSIRGBController.cpp                     |
|                                           |
|  Driver for MSI-RGB lighting controller   |
|                                           |
|  Logic adapted from:                      |
|     https://github.com/nagisa/msi-rgb     |
|                                           |
|  Adam Honse (CalcProgrammer1) 2/11/2020   |
\*-----------------------------------------*/

#include "MSIRGBController.h"
#include "dependencies/dmiinfo.h"
#include "super_io.h"

#define NUM_INVERTED_DEVICES (sizeof(inverted_devices) / sizeof(inverted_devices[0]))

/*-------------------------------------------------------------------------*\
| This is a list of DMI info for boards which need the  inversion flag set  |
\*-------------------------------------------------------------------------*/
static const char * inverted_devices[21] =
{
    "7B89",
    "7B90",
    "7B19",
    "7C02",
    "7B75",
    "7B22",
    "7B23",
    "7B24",
    "7B27",
    "7B30",
    "7B31",
    "7B51",
    "7C04",
    "7C00",
    "7B98",
    "7C22",
    "7C24",
    "7C01",
    "7C39",
    "7B86",
    "7B87"
};

MSIRGBController::MSIRGBController(int sioaddr)
{
    msi_sioaddr = sioaddr;
    DMIInfo board;

    std::string board_dmi = board.getMainboard();

    /*-----------------------------------------------------*\
    | This setup step isn't well documented                 |
    | Without this, pulsing does not work                   |
    \*-----------------------------------------------------*/
    superio_outb(msi_sioaddr, SIO_REG_LOGDEV, 0x09);

    int val_at_2c = superio_inb(msi_sioaddr, 0x2C);

    val_at_2c &= 0b11110111;
    val_at_2c |= 0b00010000;

    superio_outb(msi_sioaddr, 0x2C, val_at_2c);
    

    /*-----------------------------------------------------*\
    | Set logical device register to RGB controller         |
    \*-----------------------------------------------------*/
    superio_outb(msi_sioaddr, SIO_REG_LOGDEV, MSI_SIO_LOGDEV_RGB);

    /*-----------------------------------------------------*\
    | Test if RGB is enabled.  If it is not, enable it.     |
    \*-----------------------------------------------------*/
    int enable = superio_inb(msi_sioaddr, MSI_SIO_RGB_REG_ENABLE);

    if((enable & MSI_SIO_RGB_ENABLE_MASK) != MSI_SIO_RGB_ENABLE_MASK)
    {
        superio_outb(msi_sioaddr, MSI_SIO_RGB_REG_ENABLE, MSI_SIO_RGB_ENABLE_MASK & (enable & !MSI_SIO_RGB_ENABLE_MASK));
    }

    /*-----------------------------------------------------*\
    | Lighting enabled, no pulsing or blinking              |
    \*-----------------------------------------------------*/
    superio_outb(msi_sioaddr, MSI_SIO_RGB_REG_CFG_1, 0x00);

    /*--------------------------------------------------------------*\
    | Header on, pulse deactivated, colors inverted depending on DMI |
    \*--------------------------------------------------------------*/
    unsigned char ff_val = 0b11100010;

    for(unsigned int i = 0; i < NUM_INVERTED_DEVICES; i++)
    {
        if (board_dmi.find(std::string(inverted_devices[i])) != std::string::npos)
        {
            ff_val |= 0b00011100;
            break;
        }
    }
    superio_outb(msi_sioaddr, MSI_SIO_RGB_REG_CFG_3, ff_val);
    
    /*-----------------------------------------------------------*\
    | This seems to be related to some rainbow mode. Deactivated  |
    \*-----------------------------------------------------------*/
    superio_outb(msi_sioaddr, 0xFD, 0x00);
}

MSIRGBController::~MSIRGBController()
{

}

void MSIRGBController::SetColor(unsigned char red, unsigned char green, unsigned char blue)
{
    /*-----------------------------------------------------*\
    | The MSI RGB controller uses 4 bits per color rather   |
    | than 8.  Shift the values by 4 so that the 4 most     |
    | significant bits of each color are the new color value|
    \*-----------------------------------------------------*/
    red     = red >> 4;
    green   = green >> 4;
    blue    = blue >> 4;

    /*-----------------------------------------------------*\
    | Set logical device register to RGB controller         |
    \*-----------------------------------------------------*/
    superio_outb(msi_sioaddr, SIO_REG_LOGDEV, MSI_SIO_LOGDEV_RGB);

    /*-----------------------------------------------------*\
    | Write the colors to the color sequence registers      |
    | Only static mode is supported right now - all colors  |
    | are the same.                                         |
    \*-----------------------------------------------------*/
    superio_outb(msi_sioaddr, MSI_SIO_RGB_REG_RED_1_0, (red | (red << 4)));
    superio_outb(msi_sioaddr, MSI_SIO_RGB_REG_RED_3_2, (red | (red << 4)));
    superio_outb(msi_sioaddr, MSI_SIO_RGB_REG_RED_5_4, (red | (red << 4)));
    superio_outb(msi_sioaddr, MSI_SIO_RGB_REG_RED_7_6, (red | (red << 4)));

    superio_outb(msi_sioaddr, MSI_SIO_RGB_REG_GREEN_1_0, (green | (green << 4)));
    superio_outb(msi_sioaddr, MSI_SIO_RGB_REG_GREEN_3_2, (green | (green << 4)));
    superio_outb(msi_sioaddr, MSI_SIO_RGB_REG_GREEN_5_4, (green | (green << 4)));
    superio_outb(msi_sioaddr, MSI_SIO_RGB_REG_GREEN_7_6, (green | (green << 4)));

    superio_outb(msi_sioaddr, MSI_SIO_RGB_REG_BLUE_1_0, (blue | (blue << 4)));
    superio_outb(msi_sioaddr, MSI_SIO_RGB_REG_BLUE_3_2, (blue | (blue << 4)));
    superio_outb(msi_sioaddr, MSI_SIO_RGB_REG_BLUE_5_4, (blue | (blue << 4)));
    superio_outb(msi_sioaddr, MSI_SIO_RGB_REG_BLUE_7_6, (blue | (blue << 4)));
}