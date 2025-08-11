FROM alpine:3.18

# Install build tools AND runtime dependencies
RUN apk --no-cache add \
    g++ \
    make \
    libstdc++ \
    libgcc \
    curl \
    dumb-init && \
    rm -rf /var/cache/apk/*

RUN addgroup appuser && \
    adduser -G appuser -s /bin/sh -D appuser

RUN mkdir -p /app && \
    chown -R appuser:appuser /app

WORKDIR /app

# Copy source code
COPY --chown=appuser:appuser ./src ./src
COPY --chown=appuser:appuser ./public ./public

# Build as appuser
USER appuser
RUN cd src && make clean && make

# Move binary to app root
RUN mv src/server ./server

RUN chmod 755 /app && \
    find /app -type f -exec chmod 644 {} \; && \
    chmod +x ./server

EXPOSE 8080

HEALTHCHECK --interval=30s --timeout=10s --start-period=60s --retries=3 \
    CMD curl -f http://localhost:8080/ || exit 1

ENTRYPOINT ["/usr/bin/dumb-init", "--"]
CMD ["./server", "8080", "./public"]
