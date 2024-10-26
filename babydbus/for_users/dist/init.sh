#!/bin/bash

dbus-daemon --config-file dbus.conf
DBUS_SYSTEM_BUS_ADDRESS=unix:path=/tmp/dbus.socket /server
exec /usr/sbin/sshd -D -e -o 'UsePrivilegeSeparation=no'
