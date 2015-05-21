TEMPLATE = subdirs
!CONFIG(coverage): {
    !CONFIG(harbour): SUBDIRS += daemon
    CONFIG(harbour): SUBDIRS += harbour
}
