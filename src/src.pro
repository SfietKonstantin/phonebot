TEMPLATE = subdirs
SUBDIRS = lib plugins bin tests

bin.depends = lib
tests.depends = lib
