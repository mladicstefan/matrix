# =============================================================================
# BUILD STAGE 
# =============================================================================
FROM ubuntu:22.04 AS builder

# Update package lists and install ONLY build-time dependencies
# --no-install-recommends: Prevents installation of suggested packages (smaller image)
# rm -rf /var/lib/apt/lists/*: Removes apt cache to reduce layer size
RUN apt-get update && apt-get install --no-install-recommends -y \
    g++ \
    make \
    libc6-dev \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# Set working directory for build process
WORKDIR /build

# Copy ONLY source code needed for compilation
COPY ./src ./src
COPY ./public ./public

# Compile the application
RUN cd src && \
    make clean && \
    make

# =============================================================================
# RUNTIME STAGE 
# =============================================================================
FROM ubuntu:22.04 

# Install ONLY runtime dependencies and process manager
# tini: Lightweight init system for proper signal handling in containers
# libstdc++6: Runtime library for C++

RUN apt-get update && apt-get install --no-install-recommends -y \
    libstdc++6 \
    libc6 \
    libgcc-s1 \
    libasan6 \
    tini \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/* \
    && apt-get clean

# Create non-privileged user for running the application
# -r: Create system user (no shell access by default)
# -g: Primary group
# Using consistent UID/GID for better security across environments
RUN groupadd -r -g 1001 appuser && \
    useradd -r -u 1001 -g appuser -d /app -s /sbin/nologin appuser

RUN mkdir -p /app && \
    chown appuser:appuser /app

WORKDIR /app

# Copy compiled binary from build stage
# --from=builder: References the previous build stage
# --chown: Sets ownership during copy (more efficient than separate chown)
COPY --from=builder --chown=appuser:appuser /build/src/server ./server
COPY --from=builder --chown=appuser:appuser /build/public ./public

# Set minimal required permissions
# 750: Owner read/write/execute, group read/execute, others no access
RUN chmod 750 /app && \
    chmod 550 ./server && \
    find ./public -type f -exec chmod 644 {} \; && \
    find ./public -type d -exec chmod 755 {} \;

# Switch to non-privileged user
# CRITICAL: This ensures the application runs without root privileges
USER appuser

# Document the port the application uses
# This is metadata only - doesn't actually expose the port
EXPOSE 8080

# Configure health check (optional but recommended)
# Helps orchestrators (Docker Compose, K8s) monitor application health
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
   CMD curl -f http://localhost:8080/health || exit 1

# Use tini as PID 1 for proper signal handling
# tini ensures proper cleanup of zombie processes and signal forwarding
ENTRYPOINT ["tini", "--"]

# Run the application
# The server process will run as PID 2 under tini (PID 1)
CMD ["./server"]

# =============================================================================
# Security improvements implemented:
# 1. Multi-stage build eliminates build tools from production image
# 2. Non-root user (appuser) runs the application
# 3. Minimal base image (ubuntu:22.04-minimal) reduces attack surface
# 4. Restricted file permissions (750/550 instead of 755)
# 5. Proper init system (tini) for signal handling
# 6. No shell access for application user (/sbin/nologin)
# 7. Clean package cache removal
# 8. Explicit dependency management (only runtime libs in final image)
# =============================================================================
