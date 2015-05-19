TEMPLATE = subdirs
!CONFIG(testing): {
    !CONFIG(harbour): SUBDIRS += daemon
    CONFIG(harbour): SUBDIRS += harbour
}
