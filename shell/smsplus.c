#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <libgen.h>
#include <errno.h>
#include <sys/stat.h>

#include <GLFW/glfw3.h>

#include <ao/ao.h>

#define GB_INI_IMPLEMENTATION
#include "gb_ini.h"

#include "shared.h"

#include "smsplus.h"
#include "video.h"

settings_t settings;
gamedata_t gdata;

static GLFWwindow *window;

static ao_device *aodevice;
static ao_sample_format aoformat;
static int16_t audiobuf[96000];

static int frames = 1;

extern unsigned char *pixels;

static void audio_init() {
	ao_initialize();
	memset(&aoformat, 0, sizeof(aoformat));
	
	aoformat.bits = 16;
	aoformat.channels = 2;
	aoformat.rate = settings.audio_rate;
	aoformat.byte_format = AO_FMT_NATIVE;
	
	aodevice = ao_open_live(ao_default_driver_id(), &aoformat, NULL); // Live output
	
	if (aodevice == NULL) {
		fprintf(stderr, "Error opening audio device.\n");
		aodevice = ao_open_live(ao_driver_id("null"), &aoformat, NULL);
	}
	else {
		fprintf(stderr, "libao: %dHz, %d-bit, %d channel(s)\n", aoformat.rate, aoformat.bits, aoformat.channels);
	}
}

static void audio_deinit() {
	// Deinitialize audio
	ao_close(aodevice);
	ao_shutdown();
}

static void audio_play() {
	// Interleave the channels
	int channels = 2;
	for (int i = 0; i < (2 * channels * (settings.audio_rate / 60)); ++i) {
		audiobuf[i * 2] = snd.output[1][i];
		audiobuf[i * 2 + 1] = snd.output[0][i];
	}
	// Output
	ao_play(aodevice, (char*)audiobuf, 2 * channels * (settings.audio_rate / 60));
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// Emulator
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE); // Exit
	}
	if (key == GLFW_KEY_GRAVE_ACCENT) {
		if (action == GLFW_PRESS) { frames = settings.misc_ffspeed; }
		else if (action == GLFW_RELEASE) { frames = 1; }
	}
	/*if (key == GLFW_KEY_F5 && action == GLFW_PRESS) {
		smsp_state(0, 0); // Save Slot 0
	}
	if (key == GLFW_KEY_F6 && action == GLFW_PRESS) {
		smsp_state(1, 0); // Save Slot 1
	}
	if (key == GLFW_KEY_F7 && action == GLFW_PRESS) {
		smsp_state(0, 1); // Load Slot 0
	}
	if (key == GLFW_KEY_F8 && action == GLFW_PRESS) {
		smsp_state(1, 1); // Load Slot 1
	}*/
	if (key == GLFW_KEY_F9 && action == GLFW_PRESS) {
		smsp_video_screenshot(NULL); // Screenshot
	}
	
	// Console
	if (key == GLFW_KEY_TAB) { // SMS
		if (action == GLFW_PRESS) { input.system |= INPUT_RESET; }
		else if (action == GLFW_RELEASE) { input.system &= ~INPUT_RESET; }
	}
	if (key == GLFW_KEY_BACKSLASH) { // SMS
		if (action == GLFW_PRESS) { input.system |= INPUT_PAUSE; }
		else if (action == GLFW_RELEASE) { input.system &= ~INPUT_PAUSE; }
	}
	if (key == GLFW_KEY_ENTER) { // Game Gear
		if (action == GLFW_PRESS) { input.system |= INPUT_START; }
		else if (action == GLFW_RELEASE) { input.system &= ~INPUT_START; }
	}
	
	// Gamepad 1
	if (key == GLFW_KEY_UP) {
		if (action == GLFW_PRESS) { input.pad[0] |= INPUT_UP; }
		else if (action == GLFW_RELEASE) { input.pad[0] &= ~INPUT_UP; }
	}
	
	if (key == GLFW_KEY_DOWN) {
		if (action == GLFW_PRESS) { input.pad[0] |= INPUT_DOWN; }
		else if (action == GLFW_RELEASE) { input.pad[0] &= ~INPUT_DOWN; }
	}
	
	if (key == GLFW_KEY_LEFT) {
		if (action == GLFW_PRESS) { input.pad[0] |= INPUT_LEFT; }
		else if (action == GLFW_RELEASE) { input.pad[0] &= ~INPUT_LEFT; }
	}
	
	if (key == GLFW_KEY_RIGHT) {
		if (action == GLFW_PRESS) { input.pad[0] |= INPUT_RIGHT; }
		else if (action == GLFW_RELEASE) { input.pad[0] &= ~INPUT_RIGHT; }
	}
	
	if (key == GLFW_KEY_Z) {
		if (action == GLFW_PRESS) { input.pad[0] |= INPUT_BUTTON1; }
		else if (action == GLFW_RELEASE) { input.pad[0] &= ~INPUT_BUTTON1; }
	}
	
	if (key == GLFW_KEY_A) {
		if (action == GLFW_PRESS) { input.pad[0] |= INPUT_BUTTON2; }
		else if (action == GLFW_RELEASE) { input.pad[0] &= ~INPUT_BUTTON2; }
	}
	
	// Gamepad 2
	if (key == GLFW_KEY_I) {
		if (action == GLFW_PRESS) { input.pad[1] |= INPUT_UP; }
		else if (action == GLFW_RELEASE) { input.pad[1] &= ~INPUT_UP; }
	}
	
	if (key == GLFW_KEY_K) {
		if (action == GLFW_PRESS) { input.pad[1] |= INPUT_DOWN; }
		else if (action == GLFW_RELEASE) { input.pad[1] &= ~INPUT_DOWN; }
	}
	
	if (key == GLFW_KEY_J) {
		if (action == GLFW_PRESS) { input.pad[1] |= INPUT_LEFT; }
		else if (action == GLFW_RELEASE) { input.pad[1] &= ~INPUT_LEFT; }
	}
	
	if (key == GLFW_KEY_L) {
		if (action == GLFW_PRESS) { input.pad[1] |= INPUT_RIGHT; }
		else if (action == GLFW_RELEASE) { input.pad[1] &= ~INPUT_RIGHT; }
	}
	
	if (key == GLFW_KEY_SEMICOLON) {
		if (action == GLFW_PRESS) { input.pad[1] |= INPUT_BUTTON1; }
		else if (action == GLFW_RELEASE) { input.pad[1] &= ~INPUT_BUTTON1; }
	}
	
	if (key == GLFW_KEY_APOSTROPHE) {
		if (action == GLFW_PRESS) { input.pad[1] |= INPUT_BUTTON2; }
		else if (action == GLFW_RELEASE) { input.pad[1] &= ~INPUT_BUTTON2; }
	}
}

void smsp_state(int slot, int mode) {
	// Save and Load States
	char stpath[PATH_MAX];
	snprintf(stpath, sizeof(stpath), "%s%s.st%d", gdata.stdir, gdata.gamename, slot);
	
	FILE *fd;
	
	switch(mode) {
		case 0:
			fd = fopen(stpath, "wb");
			if (fd) {
				system_save_state(fd);
				fclose(fd);
			}
			break;
		
		case 1:
			fd = fopen(stpath, "rb");
			if (fd) {
				system_load_state(fd);
				fclose(fd);
			}
			break;
	}
}

void system_manage_sram(uint8_t *sram, int slot, int mode) {
	// Set up save file name
	FILE *fd;
	
	switch(mode) {
		case SRAM_SAVE:
			if(sms.save) {
				fd = fopen(gdata.sramfile, "wb");
				if (fd) {
					fwrite(sram, 0x8000, 1, fd);
					fclose(fd);
				}
			}
			break;
		
		case SRAM_LOAD:
			fd = fopen(gdata.sramfile, "rb");
			if (fd) {
				sms.save = 1;
				fread(sram, 0x8000, 1, fd);
				fclose(fd);
			}
			else { memset(sram, 0x00, 0x8000); }
			break;
	}
}

static GB_INI_HANDLER(smsp_ini_handler) {
//const *data, char const *section, char const *name, char const *value
	#define TEST(s, n) (strcmp(section, s) == 0 && strcmp(name, n) == 0)
	if (TEST("video", "scale")) { settings.video_scale = atoi(value); }
	else if (TEST("video", "filter")) { settings.video_filter = atoi(value); }
	else if (TEST("audio", "rate")) { settings.audio_rate = atoi(value); }
	else if (TEST("audio", "fm")) { settings.audio_fm = atoi(value); }
	else if (TEST("audio", "fmtype")) { settings.audio_fmtype = atoi(value); }
	else if (TEST("misc", "region")) { settings.misc_region = atoi(value); }
	else if (TEST("misc", "ffspeed")) { settings.misc_ffspeed = atoi(value); }
	else { return 0; }
	#undef TEST
	return 1;
}

static void smsp_gamedata_set(char *filename) {
	// Set paths, create directories
	
	// Set the game name
	snprintf(gdata.gamename, sizeof(gdata.gamename), "%s", basename(filename));
	
	// Strip the file extension off
	for (int i = strlen(gdata.gamename) - 1; i > 0; i--) {
		if (gdata.gamename[i] == '.') {
			gdata.gamename[i] = '\0';
			break;
		}
	}
	
	// Set up the sram directory
	snprintf(gdata.sramdir, sizeof(gdata.sramdir), "sram/");
#ifdef _MINGW
	if (mkdir(gdata.sramdir) && errno != EEXIST) {
#else
	if (mkdir(gdata.sramdir, 0755) && errno != EEXIST) {
#endif
		fprintf(stderr, "Failed to create %s: %d\n", gdata.sramdir, errno);
	}
	
	// Set up the sram file
	snprintf(gdata.sramfile, sizeof(gdata.sramfile), "%s%s.sav", gdata.sramdir, gdata.gamename);
	
	// Set up the state directory
	snprintf(gdata.stdir, sizeof(gdata.stdir), "state/");
#ifdef _MINGW
	if (mkdir(gdata.stdir) && errno != EEXIST) {
#else
	if (mkdir(gdata.stdir, 0755) && errno != EEXIST) {
#endif
		fprintf(stderr, "Failed to create %s: %d\n", gdata.stdir, errno);
	}
	
	// Set up the screenshot directory
#ifdef _MINGW
	if (mkdir("screenshots/") && errno != EEXIST) {
#else
	if (mkdir("screenshots/", 0755) && errno != EEXIST) {
#endif
		fprintf(stderr, "Failed to create %s: %d\n", "screenshots/", errno);
	}
}

int main (int argc, char *argv[]) {
	
	// Print Header
	fprintf(stdout, "%s %s\n", APP_NAME, APP_VERSION);
	
	if(argc < 2) {
		fprintf(stderr, "Usage: ./smsplus [FILE]\n");
		exit(1);
	}
	
	smsp_gamedata_set(argv[1]);
	
	// Check the type of ROM
	sms.console = strcmp(strrchr(argv[1], '.'), ".gg") ?
	CONSOLE_SMS : CONSOLE_GG;
	
	// Load ROM
	if(!load_rom(argv[1])) {
		fprintf(stderr, "Error: Failed to load %s.\n", argv[1]);
		exit(1);
	}
	
	fprintf(stdout, "CRC : %08X\n", cart.crc);
	fprintf(stdout, "SHA1: %s\n", cart.sha1);
	
	// Set defaults
	settings.video_scale = 2;
	settings.video_filter = 0;
	settings.audio_rate = 48000;
	settings.audio_fm = 1;
	settings.audio_fmtype = SND_EMU2413;
	settings.misc_region = TERRITORY_DOMESTIC;
	settings.misc_ffspeed = 2;
	
	// Override settings set in the .ini
	gbIniError err = gb_ini_parse("smsplus.ini", &smsp_ini_handler, &settings);
	if (err.type != GB_INI_ERROR_NONE) {
		fprintf(stderr, "Error: No smsplus.ini file found.\n");
	}
	
	// Create video buffer
	pixels = calloc(VIDEO_WIDTH_SMS * VIDEO_HEIGHT_SMS * 4, 1);
	
	// Set parameters for internal bitmap
	bitmap.width = VIDEO_WIDTH_SMS;
	bitmap.height = VIDEO_HEIGHT_SMS;
	bitmap.depth = 32;
	bitmap.granularity = 4;
	bitmap.data = pixels;
	bitmap.pitch = (bitmap.width * bitmap.granularity);
	bitmap.viewport.w = VIDEO_WIDTH_SMS;
	bitmap.viewport.h = VIDEO_HEIGHT_SMS;
	bitmap.viewport.x = 0x00;
	bitmap.viewport.y = 0x00;
	
	// Set parameters for internal sound
	snd.fm_which = settings.audio_fmtype;
	snd.fps = FPS_NTSC;
	snd.fm_clock = CLOCK_NTSC;
	snd.psg_clock = CLOCK_NTSC;
	snd.sample_rate = settings.audio_rate;
	snd.mixer_callback = NULL;
	
	sms.territory = settings.misc_region;
	if (sms.console != CONSOLE_GG) { sms.use_fm = settings.audio_fm; }
	
	// Initialize all systems and power on
	system_init();
	system_poweron();
	
	// Initialize GLFW
	if (!glfwInit()) { return -1; }
	
	// Set the GL version
	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	
	int windowwidth = VIDEO_WIDTH_SMS * settings.video_scale;
	int windowheight = VIDEO_HEIGHT_SMS * settings.video_scale;
	if (sms.console == CONSOLE_GG) {
		windowwidth = VIDEO_WIDTH_GG * settings.video_scale;
		windowheight = VIDEO_HEIGHT_GG * settings.video_scale;
	}
	
	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(windowwidth, windowheight, APP_NAME, NULL, NULL);
	
	// If the window can't be created, kill the program
	if (!window) {
		glfwTerminate();
		fprintf(stderr, "Error: Failed to create window.\n");
		return -1;
	}
	
	// Set up keyboard callback function
	glfwSetKeyCallback(window, key_callback);
	
	// Make the window's context current
	glfwMakeContextCurrent(window);
	
	// Initialize OpenGL and audio output
	ogl_init();
	audio_init();
	
	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window)) {
		// Output audio
		audio_play();
		
		// Refresh video data
		bitmap.data = pixels;
		
		// Execute frame(s)
		for (int i = 0; i < frames; i++) { system_frame(0); }
		
		// Render/Blit the Frame
		ogl_render();
		
		// Swap front and back buffers
		glfwSwapBuffers(window);
		
		// Poll for and process events
		glfwPollEvents();
	}
	
	// Deinitialize audio and video output
	ogl_deinit();
	audio_deinit();
	
	glfwTerminate();
	
	// Shut down
	system_poweroff();
	system_shutdown();
	return 0;
}
