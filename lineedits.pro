QT += widgets
requires(qtConfig(combobox))

HEADERS     = window.h \
    serial.hpp
SOURCES     = main.cpp \
              window.cpp

QMAKE_CXXFLAGS += -lboost_thread -lboost_system -pthread

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/lineedits
INSTALLS += target
