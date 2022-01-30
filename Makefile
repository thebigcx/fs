SRC=mkfs.c copy.c
TARG=$(patsubst %.c, %, $(SRC))

all: $(TARG)

%: %.c
	gcc $< lib.c -o $@

clean:
	rm $(TARG)
