
FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
    git \
    curl \
    zip \
    unzip \
    build-essential \
    cmake \
    pkg-config \
    autoconf \
    automake \
    libtool \
    bison \
    flex \
    perl \
    libssl-dev \
    zlib1g-dev \
    libbrotli-dev \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

RUN git clone https://github.com/microsoft/vcpkg.git
RUN ./vcpkg/bootstrap-vcpkg.sh
RUN ./vcpkg/vcpkg install drogon[core,ctl,redis,postgres] yaml-cpp jwt-cpp --triplet x64-linux

COPY ./backend ./backend
WORKDIR /app/backend/build
RUN cmake .. -DCMAKE_TOOLCHAIN_FILE=/app/vcpkg/scripts/buildsystems/vcpkg.cmake -DBUILD_REDIS=ON
RUN make -j$(nproc)

FROM ubuntu:22.04


RUN apt-get update && apt-get install -y --no-install-recommends \
    libssl3 \
    zlib1g \
    libbrotli1 \
    libpq5 \
    libhiredis0.14 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app


COPY --from=builder /app/backend/build/mercury_kv .
COPY --from=builder /app/backend/config.json .


EXPOSE 5555

CMD ["./mercury_kv"]
