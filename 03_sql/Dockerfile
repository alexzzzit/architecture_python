FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    git \
    cmake \
    g++ \
    libssl-dev \
    zlib1g-dev \
    libpoco-dev \
    libgtest-dev \
    ninja-build \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build

COPY includes/ ./includes/
COPY src/ ./src/
COPY tests/ ./tests/
COPY CMakeLists.txt ./CMakeLists.txt

RUN mkdir -p build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF && \
    make -j$(nproc)

FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libssl-dev \
    zlib1g-dev \
    libpoco-dev \
    curl \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/* \
    && apt-get clean

WORKDIR /app

COPY --from=builder /build/build/medical_records_api /app/

COPY openapi.yaml /app/

RUN useradd -m -u 1000 appuser && \
    chown -R appuser:appuser /app

USER appuser

EXPOSE 8080

CMD ["./medical_records_api"]