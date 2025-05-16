FROM quay.io/centos/centos:stream9

# Install required packages
RUN dnf groupinstall -y "Development Tools" && \
    dnf install -y make gcc glibc-devel && \
    dnf clean all

# Create work directory
WORKDIR /app

# Copy source code
COPY . .

# Build firmware
RUN make

# Run firmware by default
CMD ["./firmware_tpe_simulator.elf"]
