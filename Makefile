SOURCES = sample.cpp motion.cpp
TARGET = Run


OBJECTS = ${SOURCES:.cpp=.o}

INCLUDE_DIRS = -I/darwin/Linux/include -I/darwin/Framework/include

CXX = g++
CXXFLAGS += -O2 -DLINUX -Wall $(INCLUDE_DIRS)
#CXXFLAGS += -O2 -DDEBUG -DLINUX -Wall $(INCLUDE_DIRS)
#CXXFLAGS += `pkg-config opencv --cflags --libs`
LFLAGS += -lpthread -ljpeg -lrt

all: $(TARGET)

clean:
	rm -f *.a *.o $(TARGET) core *~ *.so *.lo *.swp

libclean:
	make -C /darwin/Linux/build clean

distclean: clean libclean

darwin.a:
	make -C /darwin/Linux/build

$(TARGET): darwin.a $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) /darwin/Linux/lib/darwin.a -o $(TARGET) $(LFLAGS)
	chmod 755 $(TARGET)

# useful to make a backup "make tgz"
tgz: clean
	mkdir -p backups
	tar czvf ./backups_`date +"%Y_%m_%d_%H.%M.%S"`.tgz --exclude backups *

