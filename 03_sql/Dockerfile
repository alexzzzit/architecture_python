FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    git \
    cmake \
    g++ \
    libssl-dev \
    zlib1g-dev \
    libpoco-dev \
    libpq-dev \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build

# 🔧 Копируем только нужное (без tests/)
COPY includes/ ./includes/
COPY src/ ./src/
COPY CMakeLists.txt ./CMakeLists.txt

RUN mkdir -p build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc)

# ====== RUNTIME STAGE ======
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libssl-dev \
    zlib1g-dev \
    libpoco-dev \
    libpq5 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/* \
    && apt-get clean

WORKDIR /app

# 🔥 Копируем скомпилированный бинарник
COPY --from=builder /build/build/medical_records_api /app/medical_records_api
COPY openapi.yaml /app/openapi.yaml

# Создаём пользователя для безопасности
RUN useradd -m -u 1000 appuser && \
    chown -R appuser:appuser /app

USER appuser

EXPOSE 8080

# 🔥 Запускаем API
CMD ["/app/medical_records_api"]