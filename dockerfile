# Стадия сборки
FROM ubuntu:22.04 as builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libsfml-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Копируем ВСЕ как есть
COPY . .

RUN rm -rf build && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make

# Финальная стадия  
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    libsfml-audio2.5 \
    libsfml-graphics2.5 \
    libsfml-network2.5 \
    libsfml-system2.5 \
    libsfml-window2.5 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Создаем папку game
RUN mkdir -p game

# Копируем бинарник в папку game
COPY --from=builder /app/build/GameProject ./game/lol/

# Копируем только textures из src
COPY --from=builder /app/src/textures/ ./game/src/textures/

# Удаляем исходники из финального образа (опционально)
RUN rm -rf /app/src

# Запускаем из папки game
WORKDIR /app/game/lol

CMD ["./GameProject"]
