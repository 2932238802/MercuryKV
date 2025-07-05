# backend/Dockerfile (修改后)

FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    git cmake build-essential curl zip unzip pkg-config libssl-dev zlib1g-dev libbrotli-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

RUN git clone https://github.com/microsoft/vcpkg.git
RUN ./vcpkg/bootstrap-vcpkg.sh
RUN ./vcpkg/vcpkg install drogon[core,ctl,redis,postgres] --triplet x64-linux

COPY ./backend ./backend

WORKDIR /app/backend/build
RUN cmake .. -DCMAKE_TOOLCHAIN_FILE=/app/vcpkg/scripts/buildsystems/vcpkg.cmake -DBUILD_REDIS=ON
RUN make -j$(nproc)

FROM debian:bullseye-slim

RUN apt-get update && apt-get install -y libssl1.1 zlib1g libbrotli1 && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/backend/build/mercury_kv .
COPY --from=builder /app/backend/config.json .

EXPOSE 5555

CMD ["./mercury_kv", "./config.json"]
