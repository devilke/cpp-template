#!/bin/bash
# filepath: /workspaces/cpp-template/.devcontainer/setup-novnc.sh

set -e

echo "Setting up noVNC environment..."

# Create runtime directory
mkdir -p /tmp/runtime-vscode
chmod 700 /tmp/runtime-vscode

# Create log directory
mkdir -p /tmp/logs

# Function to check if a process is running
is_running() {
    pgrep -f "$1" > /dev/null
}

# Function to start a service if not running
start_service() {
    local service_name="$1"
    local command="$2"
    local log_file="$3"
    
    if ! is_running "$service_name"; then
        echo "Starting $service_name..."
        eval "$command" > "$log_file" 2>&1 &
        sleep 2
    else
        echo "$service_name is already running"
    fi
}

# Start Xvfb (Virtual framebuffer X server)
start_service "Xvfb" \
    "/usr/bin/Xvfb :1 -screen 0 1024x768x24 -ac +extension GLX +render -noreset" \
    "/tmp/logs/xvfb.log"

# Wait for X server to be ready
sleep 3

# Start window manager
export DISPLAY=:1
start_service "fluxbox" \
    "DISPLAY=:1 fluxbox" \
    "/tmp/logs/fluxbox.log"

# Start x11vnc (VNC server)
start_service "x11vnc" \
    "x11vnc -display :1 -nopw -listen localhost -xkb -ncache 10 -ncache_cr -forever -shared" \
    "/tmp/logs/x11vnc.log"

# Start websockify (WebSocket proxy for noVNC)
start_service "websockify" \
    "websockify --web /usr/share/novnc 6080 localhost:5900" \
    "/tmp/logs/novnc.log"

echo "noVNC setup complete!"
echo "Access the desktop at: http://localhost:6080"
echo "Log files are in /tmp/logs/"

# Test OpenGL
echo "Testing OpenGL..."
DISPLAY=:1 glxinfo | head -20 || echo "OpenGL test failed, but software rendering should work"