TARGET_DIR=./build
BUILD_WITH_QT6?=FALSE

all: Release

create-dir:
	[ -x  $(TARGET_DIR) ] || mkdir $(TARGET_DIR)

Debug:	create-dir
	cd $(TARGET_DIR); cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_WITH_QT6=$(BUILD_WITH_QT6) ..; make -j4 VERBOSE=1

Release:	create-dir
	cd $(TARGET_DIR); cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_WITH_QT6=$(BUILD_WITH_QT6) ..; make -j4

clean:
	rm -rf $(TARGET_DIR)/*

install:
	cd $(TARGET_DIR); make install DESTDIR=$(prefix)
