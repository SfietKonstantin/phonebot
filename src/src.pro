TEMPLATE = subdirs
SUBDIRS = lib plugins bin

plugins.depends = lib
bin.depends = lib plugins

!CONFIG(harbour): {
    SUBDIRS += tests
    tests.depends = lib plugins
}
