#!/usr/bin/env bash

# Exit on any error, undefined variables, or pipe failures
set -euo pipefail

# Configuration
CONTAINER_NAME="webserver"
IMAGE_NAME="mladicstefan/webserver"
PORT="8080"

echo "🚀 Starting secure webserver deployment..."
echo "════════════════════════════════════════════════════════════════════════════════"

# Validate prerequisites
echo "🔍 Validating environment..."

# Check if Docker is running
if ! docker info >/dev/null 2>&1; then
    echo "❌ Docker daemon is not running or accessible"
    echo "💡 Try: sudo systemctl start docker"
    echo "💡 Or add user to docker group: sudo usermod -aG docker \$USER"
    exit 1
fi

# Check if Dockerfile exists
if [[ ! -f "Dockerfile" ]]; then
    echo "❌ Dockerfile not found in current directory"
    exit 1
fi

# Stop and remove existing container (ignore errors if not exists)
echo "🛑 Stopping existing container..."
docker stop "$CONTAINER_NAME" 2>/dev/null || true
docker rm "$CONTAINER_NAME" 2>/dev/null || true

# Build Docker image
echo "🔨 Building Docker image..."
if ! docker build -t "$IMAGE_NAME" .; then
    echo "❌ Docker build failed"
    exit 1
fi

# Deploy with security hardening
echo "🔒 Deploying with security hardening..."
if docker run \
    --read-only \
    --tmpfs /tmp \
    --tmpfs /var/tmp \
    --cap-drop=ALL \
    --security-opt=no-new-privileges:true \
    --memory=2G\
    --cpus=2.0 \
    --pids-limit=100 \
    --name "$CONTAINER_NAME" \
    --restart=unless-stopped \
    -d \
    "$IMAGE_NAME"; then
    
    echo "⏳ Checking container status..."
    sleep 3
    
    # Verify deployment
    if docker ps | grep -q "$CONTAINER_NAME"; then
        echo "✅ Webserver deployed successfully!"
        echo "🌐 Access via nginx at: https://mladicstefan.com"
        echo "🏥 Local health check: curl http://127.0.0.1:${PORT}"
        echo "📊 Container stats: docker stats $CONTAINER_NAME"
        echo "📋 Container logs: docker logs $CONTAINER_NAME"
    else
        echo "❌ Deployment failed!"
        echo "📋 Container logs:"
        docker logs "$CONTAINER_NAME" 2>/dev/null || echo "No logs available"
        exit 1
    fi
else
    echo "❌ Docker run failed"
    exit 1
fi
