FROM ubuntu:20.04
RUN apt-get update && apt-get -y install dbus libglib2.0-0 systemd openssh-server

# ID:user / Password: user
RUN useradd -m user \
	-p '$6$BlkXIGewNMlTaA7W$jt1XL.Tc2tRbhIxOcgz5aoTtax/rs1HcNXXa3YZvo4cvblXrPHfJpKaMh/4PZqoNxgvp0qOgB037oskLb/t7C0' \
	-s '/bin/bash'

COPY server dbus.conf init.sh flag.txt /

# root can read
RUN chmod 0600 /flag.txt && \
# all users can read, root can modify
	chmod 0644 /dbus.conf && \
# all users can read & execute, root can modify
	chmod 0755 /server /init.sh && \
# users cannot enumerate tmp folders
	chmod 1733 /tmp /var/tmp /dev/shm && \
# no need to change password
	rm /usr/bin/passwd && \
# ssh server support
	mkdir -p /run/sshd

CMD /init.sh