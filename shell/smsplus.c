#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <GLFW/glfw3.h>

#include <ao/ao.h>

#define GB_INI_IMPLEMENTATION
#include "gb_ini.h"

#include "shared.h"

#include "smsplus.h"
#include "video.h"

char game_name[PATH_MAX];
settings_t settings;

static GLFWwindow *window;

static ao_device *aodevice;
static ao_sample_format aoformat;
static int16_t audiobuf[96000];

static int skip = 0;

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
	char stpathbuf[PATH_MAX];
	char stpath[PATH_MAX];
	snprintf(stpathbuf, sizeof(stpathbuf), "%s", game_name);
	// strip the . and extention off the filename for saving
	for (int i = strlen(stpathbuf)-1; i > 0; i--) {
		if (stpathbuf[i] == '.') { stpathbuf[i] = '\0'; break; }
	}
	snprintf(stpath, sizeof(stpath), "%s.st%d", stpathbuf, slot);
	
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
	char savenamebuf[PATH_MAX];
	char savename[PATH_MAX];
	snprintf(savenamebuf, sizeof(savenamebuf), "%s", game_name);
	// strip the . and extention off the filename for saving
	for (int i = strlen(savenamebuf)-1; i > 0; i--) {
		if (savenamebuf[i] == '.') { savenamebuf[i] = '\0'; break; }
	}
	snprintf(savename, sizeof(savename), "%s.sav", savenamebuf);
	
	FILE *fd;
	
	switch(mode) {
		case SRAM_SAVE:
			if(sms.save) {
				fd = fopen(savename, "wb");
				if (fd) {
					fwrite(sram, 0x8000, 1, fd);
					fclose(fd);
				}
			}
			break;
		
		case SRAM_LOAD:
			fd = fopen(savename, "rb");
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
	else if (TEST("audio", "rate")) { settings.audio_rate = atoi(value); }
	else if (TEST("audio", "fm")) { settings.audio_fm = atoi(value); }
	else if (TEST("audio", "fmtype")) { settings.audio_fmtype = atoi(value); }
	else if (TEST("misc", "region")) { settings.misc_region = atoi(value); }
	else { return 0; }
	#undef TEST
	return 1;
}

int main (int argc, char *argv[]) {
	
	if(argc < 2) {
		fprintf(stderr, "No filename specified.\n");
		exit(1);
	}
	
    snprintf(game_name, sizeof(game_name), "%s", argv[1]);
	
	// Load ROM
	if(!load_rom(game_name)) {
		fprintf(stderr, "Error: Failed to load %s.\n", game_name);
		exit(1);
	}
	
	// Set defaults
	settings.video_scale = 2;
	settings.audio_rate = settings.audio_rate;
	settings.audio_fm = 1;
	settings.audio_fmtype = SND_EMU2413;
	settings.misc_region = TERRITORY_DOMESTIC;
	
	// Override settings set in the .ini
	gbIniError err = gb_ini_parse("smsplus.ini", &smsp_ini_handler, &settings);
	if (err.type != GB_INI_ERROR_NONE) { return 1; }
	
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
	
	// Initialize all systems and power on
	system_init();
	
	sms.territory = settings.misc_region;
	sms.use_fm = settings.audio_fm;
	
	system_poweron();
	
	// Initialize GLFW
	if (!glfwInit()) { return -1; }

	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(VIDEO_WIDTH_SMS * settings.video_scale, VIDEO_HEIGHT_SMS * settings.video_scale, "SMS Plus", NULL, NULL);
	
	// If the window can't be created, kill the program
	if (!window) {
		glfwTerminate();
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
		
		// Execute a frame
		system_frame(skip);
		
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
