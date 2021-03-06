TARGET_DIR=./build

all: Release

create-dir:
	[ -x  $(TARGET_DIR) ] || mkdir $(TARGET_DIR)

Debug:	create-dir
	cd $(TARGET_DIR); cmake -DCMAKE_BUILD_TYPE=Debug ..; make -j4 VERBOSE=1

Release:	create-dir
	cd $(TARGET_DIR); cmake -DCMAKE_BUILD_TYPE=Release ..; make -j4

clean:
	rm -rf $(TARGET_DIR)/*

install:
	cd $(TARGET_DIR); make install DESTDIR=$(prefix)
