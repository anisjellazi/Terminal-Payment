FROM gcc:12
WORKDIR /app
COPY . .
RUN make
CMD ["./firmware_tpe_simulator.elf"]
