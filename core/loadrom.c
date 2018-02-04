/*
    loadrom.c --
    File loading and management.
*/

#include "shared.h"

typedef struct {
    uint32_t crc;
    int mapper;
    int display;
    int territory;
    char *name;
} rominfo_t;

rominfo_t game_list[] = {
    {0x17AB6883, MAPPER_NONE  , DISPLAY_NTSC, TERRITORY_EXPORT, "FA Tetris (KR)"                },
    {0x61E8806F, MAPPER_NONE  , DISPLAY_NTSC, TERRITORY_EXPORT, "Flash Point (KR)"              },
    {0x192949D5, MAPPER_KOREA2, DISPLAY_NTSC, TERRITORY_EXPORT, "Janggun-iuo Adeul (KR)"        },
    {0xA05258F5, MAPPER_KOREA , DISPLAY_NTSC, TERRITORY_EXPORT, "Won-Si-In (KR)"                },
    {0x83F0EEDE, MAPPER_KOREA , DISPLAY_NTSC, TERRITORY_EXPORT, "Street Master (KR)"            },
    {0x445525E2, MAPPER_KOREA , DISPLAY_NTSC, TERRITORY_EXPORT, "Penguin Adventure (KR)"        },
    {0x29822980, MAPPER_CODIES, DISPLAY_PAL,  TERRITORY_EXPORT, "Cosmic Spacehead"              },
    {0xB9664AE1, MAPPER_CODIES, DISPLAY_PAL,  TERRITORY_EXPORT, "Fantastic Dizzy"               },
    {0xA577CE46, MAPPER_CODIES, DISPLAY_PAL,  TERRITORY_EXPORT, "Micro Machines"                },
    {0x8813514B, MAPPER_CODIES, DISPLAY_PAL,  TERRITORY_EXPORT, "Excellent Dizzy (Proto)"       },
    {0xAA140C9C, MAPPER_CODIES, DISPLAY_PAL,  TERRITORY_EXPORT, "Excellent Dizzy (Proto - GG)"  }, 
    {-1        , -1           , -1         ,  -1               , NULL                           },
};

int load_rom(char *filename)
{
    int i;
    int size;

    if(cart.rom)
    {
        free(cart.rom);
        cart.rom = NULL;
    }

	FILE *fd = NULL;

	fd = fopen(filename, "rb");
	if(!fd) return 0;

	/* Seek to end of file, and get size */
	fseek(fd, 0, SEEK_END);
	size = ftell(fd);
	fseek(fd, 0, SEEK_SET);

	cart.rom = malloc(size);
	if(!cart.rom) return 0;
	fread(cart.rom, size, 1, fd);

	fclose(fd);

    /* Don't load games smaller than 16K */
    if(size < 0x4000) return 0;

    /* Take care of image header, if present */
    if((size / 512) & 1)
    {
        size -= 512;
        memmove(cart.rom, cart.rom + 512, size);
    }

    cart.pages = (size / 0x4000);
    cart.crc = crc32(0L, cart.rom, size);
    
    const char *shabuf = sha1(cart.rom, sizeof(cart.rom));
    snprintf(cart.sha1, sizeof(cart.sha1), "%s", shabuf);

    /* Assign default settings (US NTSC machine) */
    cart.mapper     = MAPPER_SEGA;
    sms.display     = DISPLAY_NTSC;
    sms.territory   = TERRITORY_EXPORT;

    /* Look up mapper in game list */
    for(i = 0; game_list[i].name != NULL; i++)
    {
        if(cart.crc == game_list[i].crc)
        {
            cart.mapper     = game_list[i].mapper;
            sms.display     = game_list[i].display;
            sms.territory   = game_list[i].territory;
        }
    }

    system_assign_device(PORT_A, DEVICE_PAD2B);
    system_assign_device(PORT_B, DEVICE_PAD2B);

    return 1;
}

