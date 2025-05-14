# Copy the content of the version controlled .vscode
cp -r /home/GunnsDevcontainer/.vscode /home/.vscode

# Avoid "dubious ownership" git error
git config --global --add safe.directory /home/GunnsDevcontainer

# Make scripts executable by VSCode tasks
chmod +x /home/GunnsDevcontainer/scripts/*.sh