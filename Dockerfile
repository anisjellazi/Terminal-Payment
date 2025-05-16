FROM centos:7
RUN yum groupinstall -y "Development Tools" && yum install -y glibc-devel make gcc
WORKDIR /app
COPY . .
RUN make
CMD ["./firmware_tpe_simulator.elf"]
