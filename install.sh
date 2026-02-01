#!/bin/bash

# Define variables
APP_NAME="SkyExplorer"
APP_PATH="/usr/local/bin/SkyExplorer"
DESKTOP_FILE="/usr/share/applications/SkyExplorer.desktop"
TERMINAL_CMD="konsole"  # Default terminal for Kubuntu

# Check if the app file exists
if [ ! -f "./app" ]; then
    echo "Error: './app' file not found!"
    exit 1
fi

# Copy the app to /usr/local/bin
echo "Installing SkyExplorer to $APP_PATH..."
sudo cp "./app" "$APP_PATH"
sudo chmod +x "$APP_PATH"

# Create the .desktop file
echo "Creating .desktop file for SkyExplorer..."
sudo bash -c "cat > '$DESKTOP_FILE' << 'EOF'
[Desktop Entry]
Name=SkyExplorer
Exec=$APP_PATH
Type=Application
Terminal=true
Categories=Utility;
Path=/usr/local/bin
EOF"

# Set permissions for the .desktop file
sudo chmod 644 "$DESKTOP_FILE"

echo "Installation complete! You can now search for '$APP_NAME' in your application menu."
