CONFIG += c++11
CONFIG(coverage) {
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS_DEBUG += -lgcov -coverage
}
