CC = cc
LDFLAGS =
CFLAGS = -g
FLAGS =	-Icore \
	-Wall \
	-DLSB_FIRST \

ifeq ($(OS),Windows_NT)
	FLAGS += -D_MINGW
endif

LIBS	=	-lm -ldl -lpthread $(shell pkg-config --libs glfw3 epoxy)

OBJ	=	obj/z80.o \
		obj/sms.o \
		obj/pio.o \
		obj/memz80.o \
		obj/render.o \
		obj/tms.o \
		obj/vdp.o \
		obj/system.o \
		obj/error.o

OBJ	+=	obj/state.o	\
		obj/loadrom.o \
		obj/hash.o

OBJ	+=	obj/sound.o \
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
		@mkdir -p obj/
		$(CC) -c $< -o $@ $(CFLAGS) $(FLAGS)

obj/%.o: shell/%.c shell/%.h
		$(CC) -c $< -o $@ $(CFLAGS) $(FLAGS)

clean:
	rm -f obj/*.o
	rm -f smsplus
