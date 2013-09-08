## Desktop
LIBS = -lGLESv2 -lEGL -lX11 -lm -lpthread 
NAME =	particles
OBJ = 	src/main.o		\
	src/Egl.o		\
	src/utils.o		\
	src/ParticleEngine.o	\
	src/ShaderProgram.o	\
	src/GlApp.o

CFLAGS	= -Iinc -W  -c -O3 -pipe -DEGL_USE_X11 -Wno-unused-result -Wno-unused-parameter

#BlurFS.glsl  ParticleFS.glsl  ParticleVS.glsl  ScreenFS.glsl  ScreenVS.glsl

SHADERS	=   ../assets/BlurFS.glsl	\
	    ../assets/ParticleFS.glsl	\
	    ../assets/ScreenFS.glsl	\
	    ../assets/ParticleVS.glsl	\
	    ../assets/ScreenVS.glsl

SHADERS_OBJ =	../assets/BlurFS.glsl.o		\
		../assets/ParticleFS.glsl.o	\
		../assets/ScreenFS.glsl.o	\
		../assets/ParticleVS.glsl.o	\
		../assets/ScreenVS.glsl.o
		
android:
	@/bin/sh compile.sh
			
all: $(NAME)
	@echo Done

%.o : %.cpp
	@echo ++ $<
	@g++ $(CFLAGS) $< -o $@

$(SHADERS):
	@ld -r -b binary $@ -o $@.o

$(NAME): $(OBJ) $(SHADERS)
	@echo Linking
	@g++ -o $@ $(OBJ) $(SHADERS_OBJ) $(LIBS)

clean:
	@echo "Cleaning..."
	@rm -fv $(OBJ) $(SHADERS_OBJ)
fclean:
	@rm -fv $(NAME) 
re: fclean clean all

.PHONY: all fclean clean re $(SHADERS) android

 
