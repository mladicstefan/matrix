#!/bin/bash

echo "🚀 Starting secure webserver deployment..."
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

echo "🛑 Stopping existing container..."
docker stop webserver 2>/dev/null
docker rm webserver 2>/dev/null

echo "📦 Building Docker image..."
docker build -t mladicstefan/webserver .

echo "🔒 Deploying with security hardening..."
docker run \
  --read-only \
  --tmpfs /tmp \
  --tmpfs /var/tmp \
  --cap-drop=ALL \
  --security-opt=no-new-privileges:true \
  -p 127.0.0.1:8080:8080 \
  --name webserver \
  -d \
  mladicstefan/webserver

echo "⚡ Checking container status..."
sleep 2
if docker ps | grep -q webserver; then
    echo "✅ Webserver deployed successfully!"
    echo "🌐 Access via nginx at: https://mladicstefan.com"
    echo "🔍 Local health check: curl http://127.0.0.1:8080"
else
    echo "❌ Deployment failed!"
    echo "📋 Container logs:"
    docker logs webserver
    exit 1
fi

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "🎉 Deployment complete!"
