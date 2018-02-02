CC = cc
LDFLAGS =
FLAGS =	-Icore \
		-Wall -g \
		-DLSB_FIRST \
		#-O3 -fomit-frame-pointer -ffast-math

LIBS = -lm -lz $(shell pkg-config --libs glfw3 epoxy ao)

OBJ =	obj/z80.o	\
		obj/sms.o \
		obj/pio.o \
		obj/memz80.o \
		obj/render.o \
		obj/tms.o \
		obj/vdp.o \
		obj/system.o \
		obj/error.o

OBJ	+=	obj/state.o	\
		obj/loadrom.o

OBJ	+=  obj/sound.o \
		obj/sn76489.o \
		obj/emu2413.o \
		obj/ym2413.o \
		obj/fmintf.o \
		obj/stream.o

OBJ	+=	obj/smsplus.o \
		obj/video.o

all: smsplus

smsplus: $(OBJ)
		$(CC) -o smsplus $(OBJ) $(LIBS) $(LDFLAGS)

obj/%.o: core/%.c core/%.h
		$(CC) -c $< -o $@ $(FLAGS)

obj/%.o: shell/%.c shell/%.h
		$(CC) -c $< -o $@ $(FLAGS)

clean:
	rm -f obj/*.o
	rm -f smsplus
