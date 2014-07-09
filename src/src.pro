TEMPLATE = subdirs
SUBDIRS = lib plugins bin tests

plugins.depends = lib
bin.depends = lib plugins
tests.depends = lib plugins
