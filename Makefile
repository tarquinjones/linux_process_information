CC = gcc

CFLAGS = -Wall

INCLUDES = -I./include

SRCS = src/process_information.c src/process_details.c src/string_handler.c src/output_handler.c

OBJS = $(SRCS:.c=.o)

MAIN = proc_info

.PHONY: depend clean

all:    $(MAIN)
	@echo Compiled to process_listing 

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) src/*.o *~ $(MAIN)

